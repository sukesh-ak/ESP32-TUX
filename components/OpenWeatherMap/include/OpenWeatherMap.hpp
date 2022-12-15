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

/*
{
  "coord": {
    "lon": 77.6033,
    "lat": 12.9762
  },
  "weather": [
    {
      "id": 801,
      "main": "Clouds",
      "description": "few clouds",    // "overcast clouds"
      "icon": "02n"                   // 04n
    }
  ],
  "base": "stations",
  "main": {
    "temp": 19.8,
    "feels_like": 19.4,
    "temp_min": 18.9,
    "temp_max": 19.8,
    "pressure": 1017,     // hpa
    "humidity": 60,       // percentage
    "sea_level": 1015,    // hpa
    "grnd_level": 912     // hpa
  },
  "visibility": 6000,
  "wind": {
    "speed": 1.54,        //  m/s
    "deg": 360            // 
  },
  "clouds": {
    "all": 20       // percentage
  },
  "dt": 1668704139,
  "sys": {
    "type": 1,
    "id": 9205,
    "country": "IN",
    "sunrise": 1668646147,
    "sunset": 1668687606
  },
  "timezone": 19800,
  "id": 1277333,
  "name": "Bengaluru",
  "cod": 200
}
*/

#ifndef TUX_OWM_H_
#define TUX_OWM_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <inttypes.h>

#include "esp_log.h"
#include <fstream>
#include "esp_http_client.h"
#include <cJSON.h>
using namespace std;
#include "SettingsConfig.hpp"

class OpenWeatherMap
{
    public:
        string LocationName;
        float Temperature;     // 19.8
        float TemperatureHigh;
        float TemperatureLow;
        float TemperatureFeelsLike;
        int Pressure;
        int Humidity;
        int SeaLevel;
        int GroundLevel;

        char TemperatureUnit;   // '' / 'F' / 'C'
        string WeatherIcon;

        /* Constructor */
        OpenWeatherMap();

        /* HTTPS request to the Weather API */
        void request_weather_update();

        /* Handle json response */

    private:
        SettingsConfig *cfg;
        string cfg_filename;  /* Settings config filename*/

        string file_name; /* Weather cache filename */
        string jsonString;

        cJSON *root;
        cJSON *maininfo;
        cJSON *coord;
        cJSON *weather;
        cJSON *clouds;
        cJSON *wind;
        cJSON *sys;

        /* Load data from cache file to the instance */
        void load_json();

        /* Read cache json from flash/sdcard */
        void read_json();

        /* Write cache json on flash/sdcard */
        void write_json();

        esp_err_t request_json_over_http();        
        esp_err_t request_json_over_https();
        
    protected:

};

#endif