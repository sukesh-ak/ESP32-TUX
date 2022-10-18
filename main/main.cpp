
#include <stdio.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

static const char *TAG = "lvgl_gui";
//#define ENABLE_TEST_TIMER   // Enable/Disable TIMER used for testing

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

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
#include <gui.hpp>
//#include <page_wifi_config.hpp>

#ifdef ENABLE_TEST_TIMER
static void once_timer_callback(void* arg);
static void periodic_timer_callback(void* arg);
static void lv_update_battery(uint batval);

static bool wifi_on = false;
static int battery_value = 0;
#endif

extern "C" { void app_main(); }

void app_main(void)
{
    lcd.init();        // Initialize LovyanGFX
    lv_init();         // Initialize lvgl
    if (lv_display_init() != ESP_OK) // Configure LVGL
    {
        ESP_LOGE(TAG, "LVGL setup failed!!!");
    }

#ifdef ENABLE_TEST_TIMER
/*********************** [START] TIMERS FOR TESTING *********************/

    // Timer which trigger only once
    const esp_timer_create_args_t once_timer_args = {
            .callback = &once_timer_callback,
            .name = "once"
    };
    esp_timer_handle_t once_timer;
    ESP_ERROR_CHECK(esp_timer_create(&once_timer_args, &once_timer));
    ESP_ERROR_CHECK(esp_timer_start_once(once_timer, 5000000)); 

    // Timer which trigger periodically
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            .name = "periodic"
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000000));
  
  /*********************** [END] TIMERS FOR TESTING *********************/
#endif
    lv_setup_styles();


    draw_ui();


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

/*
    // How to stop and delete the timers
    ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
    ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));
    ESP_ERROR_CHECK(esp_timer_stop(once_timer));
    ESP_ERROR_CHECK(esp_timer_delete(once_timer));
*/    
}

#ifdef ENABLE_TEST_TIMER
static void once_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Once timer, time since boot: %lld us", time_since_boot);

    // Rotating the screen 180 deg just once
    lvgl_acquire();
    lv_disp_set_rotation(disp, LV_DISP_ROT_90);
    lvgl_release();
}

static void periodic_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer, time since boot: %lld us", time_since_boot);

    if (battery_value>100) battery_value=0;
    // Just blinking the Wifi icon between GREY & BLUE
    if (wifi_on)
    {
        lvgl_acquire();
        lv_style_set_text_color(&style_wifi, lv_palette_main(LV_PALETTE_BLUE));
        lv_obj_refresh_style(icon_wifi, LV_PART_ANY, LV_STYLE_PROP_ANY);

        lv_update_battery(battery_value);
        lvgl_release();
        wifi_on = !wifi_on;
        battery_value+=10;
    }
    else
    {
        lvgl_acquire();
        lv_style_set_text_color(&style_wifi, lv_palette_main(LV_PALETTE_GREY));
        lv_obj_refresh_style(icon_wifi, LV_PART_ANY, LV_STYLE_PROP_ANY);
        
        lv_update_battery(battery_value);
        lvgl_release();
        wifi_on = !wifi_on;
        battery_value+=10;
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
#endif