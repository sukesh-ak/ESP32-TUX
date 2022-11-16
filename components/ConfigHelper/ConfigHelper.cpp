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
Config Json format:
{
    "devicename": "yow"
    "settings": {
        "brightness":128,       // 0-255
        "theme":"dark",         // dark/light ???
        "timezone":"+5:30",     // IST
        "weather": "bangalore,india"
    }
}
*/

#include "ConfigHelper.hpp"
static const char* TAG = "ConfigHelper";

ConfigHelper::ConfigHelper()
{
    // If load is not called, these are the default values
    DeviceName = "MYDEVICE";
    Brightness = 128;                       // 0-255
    TimeZone = "+5:30";           
    CurrentTheme = "dark";                  // light / theme / ???
    WeatherLocation = "bangalore, India";
    StorageType = CONFIG_STORE_NONE;
}

// Decide storage later
void ConfigHelper::load_config()
{
    ESP_LOGI(TAG,"******************* Loading JSON *******************");
    switch (StorageType)
    {
        case CONFIG_STORE_SPIFF:
            // Code to load json string to jsonString from SPIFF partition
            {
                ifstream jsonfile("/spiffs/config.json");
                if (!jsonfile.is_open())
                {
                    ESP_LOGE(TAG,"Config File open for read failed");
                    save_config();  // create file with default values
                }

                jsonString.assign((std::istreambuf_iterator<char>(jsonfile)),
                            (std::istreambuf_iterator<char>()));

                jsonfile.close();
            }
            break;
        case CONFIG_STORE_FAT:
            // Code to load json string to jsonString from FAT partition
            break;
        case CONFIG_STORE_SDCARD:
            // Code to load json string to jsonString from SD CARD
            {
                ifstream jsonfile("/spiffs/config.json");
                if (!jsonfile.is_open())
                {
                    ESP_LOGE(TAG,"Config File open for read failed");
                    save_config();  // create file with default values
                }

                jsonString.assign((std::istreambuf_iterator<char>(jsonfile)),
                            (std::istreambuf_iterator<char>()));

                jsonfile.close();                
            }
            break;
        case CONFIG_STORE_NONE:
            // default is set to CONFIG_STORE_NONE - do not persist
            break;
    }

    // Read json variable and parse to cjson object
    root = cJSON_Parse(jsonString.c_str());

    // Get root element item
	settings = cJSON_GetObjectItem(root,"settings");

	this->Brightness = cJSON_GetObjectItem(settings,"brightness")->valueint;
    this->CurrentTheme = cJSON_GetObjectItem(settings,"theme")->valuestring;
    this->TimeZone = cJSON_GetObjectItem(settings,"timezone")->valuestring;
    this->WeatherLocation = cJSON_GetObjectItem(settings,"weather")->valuestring;

    ESP_LOGI(TAG,"Loaded:\n%s",jsonString.c_str());
    //printf("%s\n",jsonString.c_str());

    // Cleanup
    cJSON_Delete(root);
}

void ConfigHelper::save_config()
{    
    //printf("\n::******************* Saving json\n\n");
    ESP_LOGI(TAG,"******************* Saving JSON *******************");
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
	cJSON_AddStringToObject(settings,"weather",this->WeatherLocation.c_str());   

    // Save json string to variable
    jsonString = cJSON_Print(root);   // back to string
    
    // Cleanup
    cJSON_Delete(root);

    ESP_LOGI(TAG,"Saved:\n%s",jsonString.c_str());

    // Save to peristant storage
    switch (StorageType)
    {
        case CONFIG_STORE_SPIFF:
            // Code to save jsonString to SPIFF partition
            {
            // FILE* f = fopen("/spiffs/config.json", "w");
            // if (f == NULL) {
            //     ESP_LOGE(TAG, "Failed to open /spiffs/readme.txt for writing");
            //     return;
            // }
            // fprintf(f,jsonString.c_str());
            // fclose(f);
            ofstream jsonfile("/spiffs/config.json");
            if (!jsonfile.is_open())
            {
                ESP_LOGE(TAG,"Config File open for write failed");
                return ;//ESP_FAIL;
            }
            jsonfile << jsonString;
            jsonfile.flush();
            jsonfile.close();
            }


            break;
        case CONFIG_STORE_FAT:
            // Code to load json string to jsonString from FAT partition
            break;
        case CONFIG_STORE_SDCARD:
            // Code to load json string to jsonString from SD CARD
            break;
        case CONFIG_STORE_NONE:
            // default is set to CONFIG_STORE_NONE - do not persist
            break;
    }     
}

ConfigHelper::~ConfigHelper()
{
    // Destructor
}