/* Advanced HTTPS OTA example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "ota.h"

#if CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
#include "esp_efuse.h"
#endif

#if CONFIG_OTA_CONNECT_WIFI
#include "esp_wifi.h"
#endif

static const char *TAG = "OTA";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

char ota_reason[50];
#define OTA_URL_SIZE 256

static esp_err_t validate_image_header(esp_app_desc_t *new_app_info)
{
    if (new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
    }

#ifndef CONFIG_OTA_SKIP_VERSION_CHECK
    if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0) {
        ESP_LOGW(TAG, "Current running version is the same as a new. We will not continue the update.");
        
        strcpy(ota_reason,"No firmware updates found!");
        ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_ABORTED, ota_reason,sizeof(ota_reason), portMAX_DELAY));          
        return ESP_FAIL;
    }
#endif

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    /**
     * Secure version check from firmware image header prevents subsequent download and flash write of
     * entire firmware image. However this is optional because it is also taken care in API
     * esp_https_ota_finish at the end of OTA update procedure.
     */
    const uint32_t hw_sec_version = esp_efuse_read_secure_version();
    if (new_app_info->secure_version < hw_sec_version) {
        ESP_LOGW(TAG, "New firmware security version is less than eFuse programmed, %d < %d", new_app_info->secure_version, hw_sec_version);
        
        strcpy(ota_reason,"New firmware security version is less than eFuse programme!");
        ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_ABORTED, ota_reason,sizeof(ota_reason), portMAX_DELAY));         
        return ESP_FAIL;
    }
#endif

    return ESP_OK;
}

static esp_err_t _http_client_init_cb(esp_http_client_handle_t http_client)
{
    esp_err_t err = ESP_OK;
    /* Uncomment to add custom headers to HTTP request */
    // err = esp_http_client_set_header(http_client, "Custom-Header", "Value");
    return err;
}

void run_ota_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA");
    
    // Notify about TUX_EVENT_OTA_STARTED event 
    strcpy(ota_reason,"Starting...");
    ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_STARTED, ota_reason,sizeof(ota_reason), portMAX_DELAY));  

#if defined(CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE)
    /**
     * We are treating successful WiFi connection as a checkpoint to cancel rollback
     * process and mark newly updated firmware image as active. For production cases,
     * please tune the checkpoint behavior per end application requirement.
     */
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
                ESP_LOGI(TAG, "App is valid, rollback cancelled successfully");
                
                strcpy(ota_reason,"App is valid, rollback cancelled successfully");
                ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_ROLLBACK, ota_reason,sizeof(ota_reason), portMAX_DELAY));                 
            } else {
                ESP_LOGE(TAG, "Failed to cancel rollback");
                
                strcpy(ota_reason,"Failed to cancel rollback");
                ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_FAILED, ota_reason,sizeof(ota_reason), portMAX_DELAY));                 
                
            }
        }
    }
#endif

#if CONFIG_OTA_CONNECT_WIFI
#if !CONFIG_BT_ENABLED
    /* Ensure to disable any WiFi power save mode, this allows best throughput
     * and hence timings for overall OTA operation.
     */
    esp_wifi_set_ps(WIFI_PS_NONE);
#else
    /* WIFI_PS_MIN_MODEM is the default mode for WiFi Power saving. When both
     * WiFi and Bluetooth are running, WiFI modem has to go down, hence we
     * need WIFI_PS_MIN_MODEM. And as WiFi modem goes down, OTA download time
     * increases.
     */
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
#endif // CONFIG_BT_ENABLED
#endif // CONFIG_OTA_CONNECT_WIFI

    esp_err_t ota_finish_err = ESP_OK;
    esp_http_client_config_t config = {
        .url = CONFIG_OTA_FIRMWARE_UPGRADE_URL,
        .cert_pem = (char *)server_cert_pem_start,
        .timeout_ms = CONFIG_OTA_OTA_RECV_TIMEOUT,
        .keep_alive_enable = true,
    };


#ifdef CONFIG_OTA_SKIP_COMMON_NAME_CHECK
    config.skip_cert_common_name_check = true;
#endif

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
        .http_client_init_cb = _http_client_init_cb, // Register a callback to be invoked after esp_http_client is initialized
#ifdef CONFIG_OTA_ENABLE_PARTIAL_HTTP_DOWNLOAD
        .partial_http_download = true,
        .max_http_request_size = CONFIG_OTA_HTTP_REQUEST_SIZE,
#endif
    };

    esp_https_ota_handle_t https_ota_handle = NULL;
    esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "OTA Begin failed");
        
        strcpy(ota_reason,"Begin failed!");
        ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_FAILED, ota_reason,sizeof(ota_reason), portMAX_DELAY));        
        vTaskDelete(NULL);
    }

    esp_app_desc_t app_desc;
    err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_https_ota_read_img_desc failed");

        strcpy(ota_reason,"Failed reading image!");
        ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_FAILED, ota_reason,sizeof(ota_reason), portMAX_DELAY));        
        goto ota_end;
    }
    err = validate_image_header(&app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "image header verification failed");
        // Not required to be reported as a failure- NO UPDATES AVAILABLE
        goto ota_end;
    }

    int counter=0;
    while (1) {
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }

        counter++;
        // esp_https_ota_perform returns after every read operation which gives user the ability to
        // monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
        // data read so far.
        int image_len_read = esp_https_ota_get_image_len_read(https_ota_handle);
        ESP_LOGD(TAG, "Image bytes read: %d", image_len_read);

        // One event trigger for every x count
        if (counter > 200)
        {
            /* Too many events generated with this. Maybe trigger event for every 10th item? */
            // Notify about TUX_EVENT_OTA_IN_PROGRESS event / Calculate and send progress percentage (or bytes)
            ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_IN_PROGRESS, &image_len_read,sizeof(image_len_read), portMAX_DELAY));        
            counter = 0; //reset
        }

    }
    
    strcpy(ota_reason,"Download completed");
    ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_IN_PROGRESS, ota_reason,sizeof(ota_reason), portMAX_DELAY));        

    if (esp_https_ota_is_complete_data_received(https_ota_handle) != true) {
        // the OTA image was not completely received and user can customise the response to this situation.
        ESP_LOGE(TAG, "Complete data not received.");

        strcpy(ota_reason,"Complete data not received");
        ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_FAILED, ota_reason,sizeof(ota_reason), portMAX_DELAY));        

    } else {
        ota_finish_err = esp_https_ota_finish(https_ota_handle);
        if ((err == ESP_OK) && (ota_finish_err == ESP_OK)) {
            ESP_LOGI(TAG, "OTA upgrade successful. Rebooting ...");

            strcpy(ota_reason,"Upgrade successful");
            ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_COMPLETED, ota_reason,sizeof(ota_reason), portMAX_DELAY));        

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            esp_restart();
        } else {
            if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED) {
                ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            }
            ESP_LOGE(TAG, "OTA upgrade failed 0x%x", ota_finish_err);
            
            strcpy(ota_reason,"Upgrade failed...???");
            ESP_ERROR_CHECK(esp_event_post(TUX_EVENTS, TUX_EVENT_OTA_FAILED, ota_reason,sizeof(ota_reason), portMAX_DELAY));
            vTaskDelete(NULL);
        }
    }

ota_end:
    esp_https_ota_abort(https_ota_handle);

    // Trigger events from the actual place to get the error message
    vTaskDelete(NULL);
}

