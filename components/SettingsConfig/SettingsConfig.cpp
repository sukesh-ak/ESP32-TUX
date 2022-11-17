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

#include "SettingsConfig.hpp"
static const char* TAG = "SettingsConfig";

SettingsConfig::SettingsConfig(string filename)
{
    // If load is not called, these are the default values
    DeviceName = "MYDEVICE";
    Brightness = 128;                       // 0-255
    TimeZone = "+5:30";           
    CurrentTheme = "dark";                  // light / theme / ???

    WeatherProvider = "OpenWeatherMaps";
    WeatherLocation = "Bangalore, India";
    WeatherAPIkey = "";
    WeatherUpdateInterval = 5 * 60;    // Every 5mins
    TemperatureUnits = WEATHER_UNITS_CELSIUS;

    file_name = filename;
}

void SettingsConfig::load_config()
{
    ESP_LOGD(TAG,"******************* Loading JSON *******************");

    if (!file_name.empty()) read_json_file();   // read into jsonString

    // Read json variable and parse to cjson object
    root = cJSON_Parse(jsonString.c_str());

    // Get root element item
	settings = cJSON_GetObjectItem(root,"settings");

	this->Brightness = cJSON_GetObjectItem(settings,"brightness")->valueint;
    this->CurrentTheme = cJSON_GetObjectItem(settings,"theme")->valuestring;
    this->TimeZone = cJSON_GetObjectItem(settings,"timezone")->valuestring;

    ESP_LOGD(TAG,"Loaded:\n%s",jsonString.c_str());

    // Cleanup
    cJSON_Delete(root);
}

void SettingsConfig::save_config()
{    
    ESP_LOGD(TAG,"******************* Saving JSON *******************");
    // Create json object
	root=cJSON_CreateObject();
    
    // set root string elements
	cJSON_AddItemToObject(root, "devicename", cJSON_CreateString(this->DeviceName.c_str()));
    
    // create settings object on root
    cJSON_AddItemToObject(root, "settings", settings=cJSON_CreateObject());

    // Create elements
	cJSON_AddNumberToObject(settings,"brightness",this->Brightness);
	cJSON_AddStringToObject(settings,"theme",this->CurrentTheme.c_str());
	cJSON_AddStringToObject (settings,"timezone",this->TimeZone.c_str());

    // Save json string to variable
    jsonString = cJSON_Print(root);   // back to string
    
    // Cleanup
    cJSON_Delete(root);

    ESP_LOGD(TAG,"Saved:\n%s",jsonString.c_str());

    // Save jsonString to peristant storage
    if (!file_name.empty()) write_json_file();
}

void SettingsConfig::read_json_file()
{
    // read json file to string    
    ifstream jsonfile(file_name);
    if (!jsonfile.is_open())
    {
        ESP_LOGE(TAG,"File open for read failed %s",file_name.c_str());
        save_config();  // create file with default values
    }

    jsonString.assign((std::istreambuf_iterator<char>(jsonfile)),
                (std::istreambuf_iterator<char>()));

    jsonfile.close();
}

void SettingsConfig::write_json_file()
{
    ofstream jsonfile(file_name);
    if (!jsonfile.is_open())
    {
        ESP_LOGE(TAG,"File open for write failed %s",file_name.c_str());
        return ;//ESP_FAIL;
    }
    jsonfile << jsonString;
    jsonfile.flush();
    jsonfile.close();
}
