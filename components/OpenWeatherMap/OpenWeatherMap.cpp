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

#include "OpenWeatherMap.hpp"
static const char* TAG = "OpenWeatherMap";

OpenWeatherMap::OpenWeatherMap()
{
    file_name = "/spiffs/weather/weather.json";
    cfg = new SettingsConfig(file_name);
    
    // setup default values for everything.
    TemperatureUnit = 'C';
    
}

/* Get Weather from OpenWeatherMap API */
void OpenWeatherMap::request_weather_update()
{
    // Load from cache file for testing '/spiffs/weather/weather.json'
    ifstream jsonfile(file_name);
    if (!jsonfile.is_open())
    {
        ESP_LOGE(TAG,"File open for read failed %s",file_name.c_str());
        //save_json();  // create file with default values
    }

    jsonString.assign((std::istreambuf_iterator<char>(jsonfile)),
                (std::istreambuf_iterator<char>()));

    jsonfile.close();    

    // Load from HTTPS request
    // https://api.openweathermap.org/data/2.5/weather?q=Bangalore,India&units=metric&APPID=

    ESP_LOGI(TAG,"Loaded:\n%s",jsonString.c_str());

    load_json();
}

void OpenWeatherMap::load_json()
{
    // 19.8°С temperature from 18.9°С to 19.8 °С, wind 1.54 m/s. clouds 20 %, 1017 hpa
    root = cJSON_Parse(jsonString.c_str());

    // name = Bengaluru / What we searched?
    LocationName = cJSON_GetObjectItem(root,"name")->valuestring;
    int root_visibility = cJSON_GetObjectItem(root,"visibility")->valueint;
    ESP_LOGW(TAG,"root: %s / %d",LocationName.c_str(), root_visibility);

    maininfo = cJSON_GetObjectItem(root,"main");
    Temperature = cJSON_GetObjectItem(maininfo,"temp")->valuedouble;
    TemperatureFeelsLike = cJSON_GetObjectItem(maininfo,"feels_like")->valuedouble;
    TemperatureLow = cJSON_GetObjectItem(maininfo,"temp_min")->valuedouble;
    TemperatureHigh = cJSON_GetObjectItem(maininfo,"temp_max")->valuedouble;
    Pressure = cJSON_GetObjectItem(maininfo,"pressure")->valueint;
    Humidity = cJSON_GetObjectItem(maininfo,"humidity")->valueint;
    
    SeaLevel = cJSON_GetObjectItem(maininfo,"sea_level")->valueint;
    GroundLevel = cJSON_GetObjectItem(maininfo,"grnd_level")->valueint;


    ESP_LOGW(TAG,"main: %3.1f°С / %3.1f°С / %3.1f°С / %3.1f°С / %d / %dhpa",
                                            Temperature, TemperatureFeelsLike,
                                            TemperatureLow, TemperatureHigh,
                                            Pressure,Humidity);

    // 1st element of the weather array. 
    // Guess for free api version only 1 (single day) available
    weather = cJSON_GetArrayItem(cJSON_GetObjectItem(root,"weather"),0);
    string weather_main = cJSON_GetObjectItem(weather,"main")->valuestring;
    string weather_description = cJSON_GetObjectItem(weather,"description")->valuestring;
    WeatherIcon = cJSON_GetObjectItem(weather,"icon")->valuestring;
    ESP_LOGW(TAG,"weather: %s / %s / %s",weather_main.c_str(), weather_description.c_str(),WeatherIcon.c_str());

    // lon / lat
    coord = cJSON_GetObjectItem(root,"coord");
    double coord_lon = cJSON_GetObjectItem(coord,"lon")->valuedouble;
    double coord_lat = cJSON_GetObjectItem(coord,"lat")->valuedouble;
    ESP_LOGW(TAG,"coord: %f / %f ",coord_lon, coord_lat);

    // all = 20 ???
    //clouds = cJSON_GetObjectItem(root,"clouds");

    // speed / degree
    wind = cJSON_GetObjectItem(root,"wind");
    double wind_speed = cJSON_GetObjectItem(wind,"speed")->valuedouble;
    int wind_deg = cJSON_GetObjectItem(wind,"deg")->valueint;
    ESP_LOGW(TAG,"wind: %3.1f m/s / %d ",wind_speed, wind_deg);

    // type / id / country / sunrise (epoc?) / sunset (epoc?)
    //sys = cJSON_GetObjectItem(root,"sys");


    // Cleanup
    cJSON_Delete(root);
}

void OpenWeatherMap::save_json()
{
    // cache json in flash to show if not online?
}