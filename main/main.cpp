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


static const char *TAG = "lvgl_gui";

// Mount SPIFF partition and print readme.txt content
#include "helper_spiff.hpp"

// Make SPIFF available to LVGL Filesystem
#include "helper_lv_fs.hpp"

// Enable one of the devices from below
#include "conf_WT32SCO1.h"              // WT32-SC01 (ESP32)
// #include "conf_WT32SCO1-Plus.h"         // WT32-SC01 Plus (ESP32-S3) with SD Card support

#include "helper_display.hpp"

#if defined(WT32_SC01_PLUS)
    #include "helper_storage.hpp"
// else // For SD card shared on TFT SPI bus
//     #include "helper_storage_shared.hpp"
#endif

// UI design
#include "gui.hpp"

static void periodic_timer_callback(lv_timer_t * timer);
static void lv_update_battery(uint batval);
static bool wifi_on = false;
static int battery_value = 0;

extern "C" void app_main(void)
{
    ESP_LOGE(TAG,"\n%s",device_info().c_str());
    //fflush(stdout);

    //Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    lcd.init();        // Initialize LovyanGFX
    lv_init();         // Initialize lvgl
    if (lv_display_init() != ESP_OK) // Configure LVGL
    {
        ESP_LOGE(TAG, "LVGL setup failed!!!");
    }
    
    // Init SPIFF & print readme.txt from the root
    init_spiff();
    print_readme_txt();

    // LV_FS integration & print readme.txt from the root
    lv_print_readme_txt();

    lvgl_acquire();
    create_splash_screen();
    lvgl_release();

    lvgl_acquire();
    lv_setup_styles();    
    show_ui();
    lvgl_release();

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

    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());

    // Status icon animation timer
    // lv_timer_t * timer_status = lv_timer_create(periodic_timer_callback, 1000,  NULL);
    // lv_timer_ready(timer_status);
}

static void periodic_timer_callback(lv_timer_t * timer)
{
    if (battery_value>100) battery_value=0;
    // Just blinking the Wifi icon between GREY & BLUE
    if (wifi_on)
    {
        lv_style_set_text_color(&style_ble, lv_palette_main(LV_PALETTE_GREY));
        lv_label_set_text(icon_wifi, FA_SYMBOL_BLE);

        lv_style_set_text_color(&style_wifi, lv_palette_main(LV_PALETTE_BLUE));
        lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);

        lv_update_battery(battery_value);
        wifi_on = !wifi_on;
        battery_value+=10;

        ESP_LOGI(TAG, "periodic_timer_callback : WiFi = ON\n");
    }
    else
    {
        lv_style_set_text_color(&style_ble, lv_palette_main(LV_PALETTE_BLUE));
        lv_label_set_text(icon_wifi, FA_SYMBOL_BLE);

        lv_style_set_text_color(&style_wifi, lv_palette_main(LV_PALETTE_GREY));
        lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
        lv_update_battery(battery_value);
        wifi_on = !wifi_on;
        battery_value+=10;

        ESP_LOGI(TAG, "periodic_timer_callback : WiFi = OFF\n");
    }
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
