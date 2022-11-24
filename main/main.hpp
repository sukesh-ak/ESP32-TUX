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

#include "SettingsConfig.hpp"

#include "wifi_prov_mgr.hpp"    // Provision and connect to Wifi
#include "helper_sntp.hpp"      // Get and set device time

// Mount SPIFF partition and print readme.txt content
#include "helper_spiff.hpp"

// Make SPIFF available to LVGL Filesystem
#include "helper_lv_fs.hpp"

// Enable one of the devices from below (shift to bsp selection later)
///#include "conf_WT32SCO1.h"              // WT32-SC01 (ESP32)

// WT32-SC01 Plus (ESP32-S3 + 8Bit Parellel) with SD Card, Audio support
//#include "conf_WT32SCO1-Plus.h"         

// Makerfabs ESP32S335D (ESP32-S3 + 16Bit Parellel) with SD Card, Audio support
#include "conf_Makerfabs_S3_PTFT.h"     

#include "helper_display.hpp"

#if defined(SD_SUPPORTED)
    /* SD Card support */
    #if defined(WT32_SC01)
        // Shared SPI Bus with Display
        #include "helper_storage_shared.hpp"
    #else
        // Display and SD card is not on same SPI bus
        #include "helper_storage.hpp"
    #endif
#endif

// UI design
#include "gui.hpp"
#include "events/tux_events.hpp"

#include "OpenWeatherMap.hpp"
#include "events/gui_events.hpp"

/* Event source periodic timer related definitions */
ESP_EVENT_DEFINE_BASE(TUX_EVENTS);

SettingsConfig *cfg;
OpenWeatherMap *owm;

static void timer_datetime_callback(lv_timer_t * timer);
static void timer_weather_callback(lv_timer_t * timer);
static void lv_update_battery(uint batval);
static void tux_ui_change_cb(void * s, lv_msg_t *m);

static string device_info();

static bool is_wifi_provisioned = false;
static bool is_wifi_connected = false;
static bool is_sdcard_enabled = false;
static bool is_dark_theme = true;

static int battery_value = 0;

static lv_timer_t * timer_datetime;
static lv_timer_t * timer_weather;

// Take your pick, here is the complete timezone list :)
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
// #define TZ_STRING "EST5EDT,M3.2.0/2,M11.1.0"    // Eastern Standard Time
// #define TZ_STRING "CST-8"                       // China Standard Time
#define TZ_STRING "UTC-05:30"                   // India Standart Time

char qr_payload[150] = {0};     // QR code data for WiFi provisioning
char ip_payload[20] = {0};      // IP Address
char ota_status[150] = {0};     // OTA status during updates
char devinfo_data[300] = {0};   // Device info
