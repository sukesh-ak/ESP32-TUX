/*
MIT License

Copyright (c) 2022 Sukesh Ashok Kumar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "nvs_flash.h"

#include <cmath>
#include <inttypes.h>
#include <string>

#include <fmt/core.h>
#include <fmt/format.h>

using namespace std ;
#include "soc/rtc.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"

static const char *TAG = "ESP32-TUX";

#include "wifi_prov_mgr.hpp"    // Provision and connect to Wifi
#include "helper_sntp.hpp"      // Get and set device time

// Mount SPIFF partition and print readme.txt content
#include "helper_spiff.hpp"

// Make SPIFF available to LVGL Filesystem
#include "helper_lv_fs.hpp"

// Enable one of the devices from below
///#include "conf_WT32SCO1.h"              // WT32-SC01 (ESP32)
#include "conf_WT32SCO1-Plus.h"         // WT32-SC01 Plus (ESP32-S3) with SD Card support

#include "helper_display.hpp"

#if defined(WT32_SC01_PLUS)
    #include "helper_storage.hpp"
// else // For SD card shared on TFT SPI bus
//     #include "helper_storage_shared.hpp"
#endif

// UI design
#include "gui.hpp"

#include "events/tux_events.hpp"

/* Event source periodic timer related definitions */
ESP_EVENT_DEFINE_BASE(TUX_EVENTS);

static void periodic_timer_callback(lv_timer_t * timer);
static void lv_update_battery(uint batval);
static bool is_wifi_connected = false;
static int battery_value = 0;
static lv_timer_t * timer_status;

// Take your pick, here is the complete timezone list :)
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
// #define TZ_STRING "EST5EDT,M3.2.0/2,M11.1.0"    // Eastern Standard Time
// #define TZ_STRING "CST-8"                       // China Standard Time
#define TZ_STRING "UTC-05:30"                   // India Standart Time

// GEt time from internal RTC and update date/time of the clock
static void update_datetime_ui()
{
    // date/time format reference => https://cplusplus.com/reference/ctime/strftime/
    time_t now;
    struct tm datetimeinfo;
    time(&now);
    localtime_r(&now, &datetimeinfo);

    // tm_year will be (1970 - 1900) = 70, if not set
    if (datetimeinfo.tm_year < 100) // Time travel not supported :P
    {
        // Date/Time is not set yet
        // ESP_LOGE(TAG, "Date/time not set yet [%d]",datetimeinfo.tm_year);    
        return;
    }

    // Date & Time formatted
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c %z", &datetimeinfo);
    //ESP_LOGW(TAG, "Date/time: %s",strftime_buf );

    // Date formatted
    strftime(strftime_buf, sizeof(strftime_buf), "%a, %e %b %Y", &datetimeinfo);
    lv_label_set_text_fmt(lbl_date,"%s",strftime_buf);

    // Time in 24hrs
    // strftime(strftime_buf, sizeof(strftime_buf), "%H:%M", datetimeinfo);

    // Time in 12hrs 
    strftime(strftime_buf, sizeof(strftime_buf), "%I:%M", &datetimeinfo);
    lv_label_set_text_fmt(lbl_time, "%s", strftime_buf);

    // 12hr clock AM/PM
    strftime(strftime_buf, sizeof(strftime_buf), "%p", &datetimeinfo);
    lv_label_set_text_fmt(lbl_ampm, "%s", strftime_buf);
}

static const char* get_id_string(esp_event_base_t base, int32_t id) {
    //if (base == TUX_EVENTS) {
        switch(id) {
            case TUX_EVENT_DATETIME_SET:
                return "TUX_EVENT_DATETIME_SET";
            case TUX_EVENT_OTA_STARTED:
                return "TUX_EVENT_OTA_STARTED";
            case TUX_EVENT_OTA_IN_PROGRESS:
                return "TUX_EVENT_OTA_IN_PROGRESS";
            case TUX_EVENT_OTA_ROLLBACK:
                return "TUX_EVENT_OTA_ROLLBACK";
            case TUX_EVENT_OTA_COMPLETED:
                return "TUX_EVENT_OTA_COMPLETED";
            case TUX_EVENT_OTA_FAILED:
                return "TUX_EVENT_OTA_FAILED";
            case TUX_EVENT_OTA_ABORTED:
                return "TUX_EVENT_OTA_ABORTED";
            case TUX_EVENT_WEATHER_UPDATED:
                return "TUX_EVENT_WEATHER_UPDATED";
            case TUX_EVENT_THEME_CHANGED:
                return "TUX_EVENT_THEME_CHANGED";
            default:
                return "TUX_EVENT_UNKNOWN";        
        }
    //} 
}

// tux event handler
static void tux_event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    //ESP_LOGW(TAG, "%s:%s: tux_event_handler", event_base, get_id_string(event_base, event_id));
    if (event_base != TUX_EVENTS) return;   // bye bye - me not invited :(

    // Handle TUX_EVENTS
    if (event_id == TUX_EVENT_DATETIME_SET) {

        // Update local timezone
        setenv("TZ", CONFIG_TIMEZONE_STRING, 1);
        tzset();    

        // update clock
        update_datetime_ui();

    } else if (event_id == TUX_EVENT_OTA_STARTED) {
        // OTA Started
        char *reason = (char*)event_data;
        lv_label_set_text_fmt(lbl_update_status, "OTA: %s", reason);

    } else if (event_id == TUX_EVENT_OTA_IN_PROGRESS) {
        // OTA In Progress - progressbar?
        int img_len_read = (int)event_data;
        ESP_LOGW(TAG, "OTA: Data read : %d", img_len_read);
        lv_label_set_text_fmt(lbl_update_status, "OTA: Data read : %d", img_len_read);

    } else if (event_id == TUX_EVENT_OTA_ROLLBACK) {
        // OTA Rollback - god knows why!
        char *reason = (char*)event_data;
        lv_label_set_text_fmt(lbl_update_status, "OTA: %s", reason);

    } else if (event_id == TUX_EVENT_OTA_COMPLETED) {
        // OTA Completed - YAY! Success
        char *reason = (char*)event_data;
        lv_label_set_text_fmt(lbl_update_status, "OTA: %s", reason);

        // wait before reboot
        vTaskDelay(3000 / portTICK_PERIOD_MS);

    } else if (event_id == TUX_EVENT_OTA_ABORTED) {
        // OTA Aborted - Not a good day for updates
        char *reason = (char*)event_data;
        lv_label_set_text_fmt(lbl_update_status, "OTA: %s", reason);

    } else if (event_id == TUX_EVENT_OTA_FAILED) {
        // OTA Failed - huh! - maybe in red color?
        char *reason = (char*)event_data;
        lv_label_set_text_fmt(lbl_update_status, "OTA: %s", reason);

    } else if (event_id == TUX_EVENT_WEATHER_UPDATED) {
        // Weather updates - summer?

    } else if (event_id == TUX_EVENT_THEME_CHANGED) {
        // Theme changes - time to play with dark & light

    }
}                          

// Wifi & IP related event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT  && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        //is_wifi_connected = true; // Use GOT_IP instead
        // Not a warning but just for highlight
        ESP_LOGW(TAG,"WIFI_EVENT_STA_CONNECTED");
        lv_style_set_text_color(&style_wifi, lv_palette_main(LV_PALETTE_BLUE));
        lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        is_wifi_connected = false;        
        lv_timer_pause(timer_status);   // stop/pause timer

        ESP_LOGW(TAG,"WIFI_EVENT_STA_DISCONNECTED");

        lv_style_set_text_color(&style_wifi, lv_palette_main(LV_PALETTE_GREY));
        lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        is_wifi_connected = true;
        lv_timer_ready(timer_status);   // start timer

        ESP_LOGW(TAG,"IP_EVENT_STA_GOT_IP");
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

        // Display IP on the footer
        //footer_message("IP: " IPSTR, IP2STR(&event->ip_info.ip));
        lv_label_set_text_fmt(lbl_update_status, "IP: " IPSTR, IP2STR(&event->ip_info.ip));
        
        // We got IP, lets update time from SNTP. RTC keeps time unless powered off
        xTaskCreate(configure_time, "config_time", 1024*4, NULL, 3, NULL);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP)
    {
        is_wifi_connected = false;
        ESP_LOGW(TAG,"IP_EVENT_STA_LOST_IP");

        // Ideally we can kick off provisioning task here.  Will test later
        xTaskCreate(provision_wifi, "wifi_prov", 1024*8, NULL, 3, NULL);
    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_START) {

    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_CRED_RECV) {

    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_CRED_FAIL) {

    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_CRED_SUCCESS) {

    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_END) {

    }
}

extern "C" void app_main(void)
{
    esp_log_level_set("wifi", ESP_LOG_WARN); // enable WARN logs from WiFi stack

    // Print device info
    ESP_LOGE(TAG,"\n%s",device_info().c_str());

    //Initialize NVS
    esp_err_t err = nvs_flash_init();
    // NVS partition contains new data format or unable to access
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    lcd.init();        // Initialize LovyanGFX
    lv_init();         // Initialize lvgl
    if (lv_display_init() != ESP_OK) // Configure LVGL
    {
        ESP_LOGE(TAG, "LVGL setup failed!!!");
    }

    // /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Register for event handler for Wi-Fi, IP related events */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    
    /* Events related to provisioning */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    // TUX EVENTS
    ESP_ERROR_CHECK(esp_event_handler_instance_register(TUX_EVENTS, ESP_EVENT_ANY_ID, tux_event_handler, NULL, NULL));

    // Init SPIFF & print readme.txt from the root
    init_spiff();
    print_readme_txt();

    // LV_FS integration & print readme.txt from the root
    lv_print_readme_txt();

/* Push LVGL/UI to its own task later*/
    // Splash screen
    lvgl_acquire();
    create_splash_screen();
    lvgl_release();

    // Wifi Provision and connection.
    // Use idf.py menuconfig to configure 
    // Use SoftAP only / BLE has some issues
    // Tuning PSRAM options visible only in IDF5, so will wait till then for BLE.
    xTaskCreate(provision_wifi, "wifi_prov", 1024*8, NULL, 3, NULL);

    // Main UI
    lvgl_acquire();
    lv_setup_styles();    
    show_ui();
    lvgl_release();
/* Push these to its own task later*/

#ifdef SD_ENABLED
    lvgl_acquire();
    // Initializing SDSPI 
    if (init_sdspi() != ESP_OK) // SD SPI
        lv_style_set_text_color(&style_storage, lv_palette_main(LV_PALETTE_RED));
    else 
        lv_style_set_text_color(&style_storage, lv_palette_main(LV_PALETTE_GREEN));
    
    lv_obj_refresh_style(icon_storage, LV_PART_ANY, LV_STYLE_PROP_ANY);
    lvgl_release();
#endif

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());

    // Status icon animation timer
    timer_status = lv_timer_create(periodic_timer_callback, 1000,  NULL);

}

static void periodic_timer_callback(lv_timer_t * timer)
{
    // Battery icon animation
    if (battery_value>100) battery_value=0;
    battery_value+=10;
    lv_update_battery(battery_value);

    update_datetime_ui();
}

static void lv_update_battery(uint batval)
{
    if (batval < 20)
    {
        lv_style_set_text_color(&style_battery, lv_palette_main(LV_PALETTE_RED));
        lv_label_set_text(icon_battery, LV_SYMBOL_BATTERY_EMPTY);
    }
    else if (batval < 50)
    {
        lv_style_set_text_color(&style_battery, lv_palette_main(LV_PALETTE_RED));
        lv_label_set_text(icon_battery, LV_SYMBOL_BATTERY_1);
    }
    else if (batval < 70)
    {
        lv_style_set_text_color(&style_battery, lv_palette_main(LV_PALETTE_DEEP_ORANGE));
        lv_label_set_text(icon_battery, LV_SYMBOL_BATTERY_2);
    }
    else if (batval < 90)
    {
        lv_style_set_text_color(&style_battery, lv_palette_main(LV_PALETTE_GREEN));
        lv_label_set_text(icon_battery, LV_SYMBOL_BATTERY_3);
    }
    else
    {
        lv_style_set_text_color(&style_battery, lv_palette_main(LV_PALETTE_GREEN));
        lv_label_set_text(icon_battery, LV_SYMBOL_BATTERY_FULL);
    }
}
