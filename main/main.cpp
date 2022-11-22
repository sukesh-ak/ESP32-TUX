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

static const char *TAG = "ESP32-TUX";
#include "main.hpp"

// GEt time from internal RTC and update date/time of the clock
static void update_datetime_ui()
{
    // If we are on another screen where lbl_time is not valid
    //if (!lv_obj_is_valid(lbl_time)) return;

    // date/time format reference => https://cplusplus.com/reference/ctime/strftime/
    time_t now;
    struct tm datetimeinfo;
    time(&now);
    localtime_r(&now, &datetimeinfo);

    // tm_year will be (1970 - 1900) = 70, if not set
    if (datetimeinfo.tm_year < 100) // Time travel not supported :P
    {
        // ESP_LOGD(TAG, "Date/time not set yet [%d]",datetimeinfo.tm_year);    
        return;
    }

    // Send update time to UI with payload using lv_msg
    lv_msg_send(MSG_TIME_CHANGED, &datetimeinfo);
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
    ESP_LOGD(TAG, "tux_event_handler => %s:%s", event_base, get_id_string(event_base, event_id));
    if (event_base != TUX_EVENTS) return;   // bye bye - me not invited :(

    // Handle TUX_EVENTS
    if (event_id == TUX_EVENT_DATETIME_SET) {

        // Update local timezone
        //setenv("TZ", cfg->TimeZone, 1);
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
    //ESP_LOGD(TAG, "%s:%s: wifi_event_handler", event_base, get_id_string(event_base, event_id));
    if (event_base == WIFI_EVENT  && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        // Not a warning but just for highlight
        ESP_LOGW(TAG,"WIFI_EVENT_STA_CONNECTED");
        lv_style_set_text_color(&style_wifi, lv_palette_main(LV_PALETTE_BLUE));
        lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        is_wifi_connected = false;        
        lv_timer_pause(timer_datetime);   // stop/pause timer

        ESP_LOGW(TAG,"WIFI_EVENT_STA_DISCONNECTED");

        lv_style_set_text_color(&style_wifi, lv_palette_main(LV_PALETTE_GREY));
        lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);

    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        is_wifi_connected = true;
        lv_timer_ready(timer_datetime);   // start timer

        ESP_LOGW(TAG,"IP_EVENT_STA_GOT_IP");
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

        //lv_label_set_text_fmt(lbl_wifi_status, "IP: " IPSTR, IP2STR(&event->ip_info.ip));
        
        // We got IP, lets update time from SNTP. RTC keeps time unless powered off
        xTaskCreate(configure_time, "config_time", 1024*4, NULL, 3, NULL);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP)
    {
        is_wifi_connected = false;
        ESP_LOGW(TAG,"IP_EVENT_STA_LOST_IP");
    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_START) {
        ESP_LOGW(TAG,"WIFI_PROV_START");
    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_CRED_RECV) {
        ESP_LOGW(TAG,"WIFI_PROV_CRED_RECV");
    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_CRED_FAIL) {
        ESP_LOGW(TAG,"WIFI_PROV_CRED_FAIL");
    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_CRED_SUCCESS) {
        ESP_LOGW(TAG,"WIFI_PROV_CRED_SUCCESS");
    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_END) {
        ESP_LOGW(TAG,"WIFI_PROV_END");
        lvgl_acquire();
        // lv_qrcode_update(prov_qr, payload, strlen(payload));
        lv_label_set_text(lbl_scan_status, "Connected to Wi-Fi sucessfully.");
        lvgl_release();        
    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_SHOWQR) {
        ESP_LOGW(TAG,"WIFI_PROV_SHOWQR");
        
        char *payload = (char*)event_data;      // get payload

        lvgl_acquire();
        lv_qrcode_update(prov_qr, payload, strlen(payload));
        lv_label_set_text(lbl_scan_status, "Install 'ESP SoftAP Prov' App & Scan");
        lvgl_release();
    }
}


extern "C" void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);      // enable DEBUG logs for this App
    //esp_log_level_set("SettingsConfig", ESP_LOG_DEBUG);    
    esp_log_level_set("wifi", ESP_LOG_WARN);    // enable WARN logs from WiFi stack

    // Print device info
    ESP_LOGE(TAG,"\n%s",device_info().c_str());

    //Initialize NVS
    esp_err_t err = nvs_flash_init();

    // NVS partition contains new data format or unable to access
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err); 

    // Init SPIFF - needed for lvgl images
    init_spiff();

#ifdef SD_ENABLED
    // Initializing SDSPI 
    if (init_sdspi() == ESP_OK) // SD SPI
    {
        sd_card_enabled = true;
    }
#endif   
//********************** CONFIG HELPER TESTING STARTS

     //cfg = new SettingsConfig("/sdcard/settings.json");    // yet to test
    cfg = new SettingsConfig("/spiffs/settings.json");
    // Save settings
    cfg->save_config();   // save default loaded settings
    // Load values
    cfg->load_config();
    // Change device name
    cfg->DeviceName = "ESP32-TUX";
    // Change brightness
    cfg->Brightness=250;
    // Save settings again
    cfg->save_config();
    cfg->load_config();

//******************************************** 
    owm = new OpenWeatherMap();
    owm->request_weather_update();
//********************** CONFIG HELPER TESTING ENDS

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

    // LV_FS integration & print readme.txt from the root
    lv_print_readme_txt();

/* Push LVGL/UI to its own task later*/
    // Splash screen
    lvgl_acquire();
    create_splash_screen();
    lvgl_release();

    // Main UI
    lvgl_acquire();
    lv_setup_styles();    
    show_ui();
    lvgl_release();
/* Push these to its own task later*/

#ifdef SD_ENABLED
    lvgl_acquire();
    // SD CARD available? 
    if (sd_card_enabled) 
        lv_style_set_text_color(&style_storage, lv_palette_main(LV_PALETTE_GREEN));
    else 
        lv_style_set_text_color(&style_storage, lv_palette_main(LV_PALETTE_RED));
    
    lv_obj_refresh_style(icon_storage, LV_PART_ANY, LV_STYLE_PROP_ANY);
    lvgl_release();
#endif

    // Wifi Provision and connection.
    // Use idf.py menuconfig to configure 
    // Use SoftAP only / BLE has some issues
    // Tuning PSRAM options visible only in IDF5, so will wait till then for BLE.
    xTaskCreate(provision_wifi, "wifi_prov", 1024*8, NULL, 3, NULL);

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());

    // Date/Time update timer - once per sec
    timer_datetime = lv_timer_create(timer_datetime_callback, 1000,  NULL);

    // Weather update timer - Once per min (60*1000)
    timer_weather = lv_timer_create(timer_weather_callback, 5 * 1000,  NULL);
}

static void timer_datetime_callback(lv_timer_t * timer)
{
    // // Battery icon animation
    // if (battery_value>100) battery_value=0;
    // battery_value+=10;
    // lv_update_battery(battery_value);

    update_datetime_ui();
}

static void timer_weather_callback(lv_timer_t * timer)
{
    // Update weather and trigger UI update
    owm->request_json_over_https();
    //owm->request_weather_update();
    //lv_msg_send(MSG_WEATHER_CHANGED, owm);
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
