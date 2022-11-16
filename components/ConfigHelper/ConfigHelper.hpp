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

#ifndef TUX_CONFIG_H_
#define TUX_CONFIG_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <inttypes.h>
#include "cJSON.h"
#include "esp_log.h"
#include <fstream>
using namespace std;


// set this up in menuconfig
typedef enum
{
    CONFIG_STORE_NONE,
    CONFIG_STORE_SPIFF,  // SPIFF partition
    CONFIG_STORE_FAT,    // FAT partition
    CONFIG_STORE_SDCARD  // On SD CARD
} config_store_t;

class ConfigHelper
{
    public:
        string DeviceName;
        uint8_t Brightness;        // 0-255
        string TimeZone;           // +5:30
        string CurrentTheme;       // dark / light
        string WeatherLocation;    // bangalore, india
        config_store_t StorageType;

        ConfigHelper();
        void load_config();
        void save_config();
        ~ConfigHelper();

    private:
        string jsonString;
        cJSON *root;
        cJSON *settings;

    protected:
};

#endif
