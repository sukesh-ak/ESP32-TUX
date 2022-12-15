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
#include "esp_tls.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

static const char* TAG = "OpenWeatherMap";

// Can Test HTTPS using Local Python server - same SSL cert used for OTA
// extern const uint8_t local_server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
// extern const uint8_t local_server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

// Since HTTP call works for weather, we skip HTTPS for now
// extern const uint8_t owm_server_cert_pem_start[] asm("_binary_owm_cert_pem_start");
// extern const uint8_t owm_server_cert_pem_end[] asm("_binary_owm_cert_pem_end");

/*
    Free OpenWeatherAPI is available one request per min. We request once every 10mins

    We pull realtime weather from the API and dump in a json local cache file (SPIFF/SDCARD/FAT)
    Then read that file to show in the UI every 5mins. 
    If device is disconnected from internet or fails, it will show the last weather update
*/

#define MAX_HTTP_OUTPUT_BUFFER 1024

// Move all these to config.json later
#define WEB_API_URL     CONFIG_WEATHER_OWM_URL //"api.openweathermap.org"
#define WEB_API_PORT    "80"    // not used unless we need custom port number
#define WEB_API_PATH "/data/2.5/weather" //?q=" CONFIG_WEATHER_LOCATION "&units=metric&APPID="

OpenWeatherMap::OpenWeatherMap()
{
    // Weather cache filename
    file_name = "/spiffs/weather/weather.json";

    // Settings filename / add these after UI has these config options
    // cfg_filename = "/spiffs/settings.json";
    // cfg = new SettingsConfig(cfg_filename);
    // cfg->load_config();

    // setup default values for everything.
#if defined(CONFIG_WEATHER_UNITS_METRIC)    
    TemperatureUnit = 'C';
#elif defined(CONFIG_WEATHER_UNITS_IMPERIAL)
    TemperatureUnit = 'F';
#else   // Standard => Kelvin
    TemperatureUnit = 'K';  // degree symbol not used
#endif
}

/* 
 * Get Weather from OpenWeatherMap API 
 * API call can fail => no wifi / connectivity issues / request limits
 * If fails, data from cache file is used.
*/
void OpenWeatherMap::request_weather_update()
{
    jsonString = "";

    // Get weather from OpenWeatherMap and update the cache file
    if (request_json_over_http() == ESP_OK) {
        ESP_LOGI(TAG,"Updating and writing into cache - weather.json");
        write_json();    // Save content of jsonString to file if success
    }
    ESP_LOGI(TAG,"Reading - weather.json");
    read_json();

    ESP_LOGI(TAG,"Loading - weather.json");
    load_json();
}

void OpenWeatherMap::load_json()
{
    ESP_LOGW(TAG,"load_json() \n%s",jsonString.c_str());

    // try
	// {

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
    
    ESP_LOGW(TAG,"main: %3.1f°С / %3.1f°С / %3.1f°С / %3.1f°С / %d / %dhpa",
                                            Temperature, TemperatureFeelsLike,
                                            TemperatureLow, TemperatureHigh,
                                            Pressure,Humidity);

/*    
    SeaLevel = cJSON_GetObjectItem(maininfo,"sea_level")->valueint;
    GroundLevel = cJSON_GetObjectItem(maininfo,"grnd_level")->valueint;
*/


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

	// }
	// catch (exception& exc)
	// {
	// 	ESP_LOGE(TAG,"Exception has occurred!");
	// }

    // Cleanup
    cJSON_Delete(root);
}

void OpenWeatherMap::read_json()
{
    // read json file to string    
    ifstream jsonfile(file_name);
    if (!jsonfile.is_open())
    {
        ESP_LOGE(TAG,"File open for read failed %s",file_name.c_str());
        //save_config();  // create file with default values
    }

    jsonString.assign((std::istreambuf_iterator<char>(jsonfile)),
                (std::istreambuf_iterator<char>()));

    jsonfile.close();    
}

void OpenWeatherMap::write_json()
{
    // cache json in flash to show if not online?
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

esp_err_t http_event_handle(esp_http_client_event_t *evt)
{
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }            
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH, Total len=%d", output_len);
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            output_len = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            break;
    }
    return ESP_OK;
}

/*
    Get OpenWeatherMaps certificate - api.openweathermap.org
    openssl s_client -showcerts -connect api.openweathermap.org:443 </dev/null
*/
esp_err_t OpenWeatherMap::request_json_over_https()
{
    ESP_LOGI(TAG, "HTTPS request to get weather");
    return ESP_OK;
}

/*
    Get OpenWeatherMaps json using http - api.openweathermap.org
*/
esp_err_t OpenWeatherMap::request_json_over_http()
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    jsonString = "";
    string queryString = "";

    // units = standard / metric / imperial
    // https://openweathermap.org/weather-data
    #if defined(CONFIG_WEATHER_UNITS_METRIC)    
            queryString = WEB_API_PATH "?q=" CONFIG_WEATHER_LOCATION "&units=metric&APPID=" CONFIG_WEATHER_API_KEY;
    #elif defined(CONFIG_WEATHER_UNITS_IMPERIAL)
            queryString = WEB_API_PATH "?q=" CONFIG_WEATHER_LOCATION "&units=imperial&APPID=" CONFIG_WEATHER_API_KEY;
    #else   // Standard => Kelvin?
            queryString = WEB_API_PATH "?q=" CONFIG_WEATHER_LOCATION "&APPID=" CONFIG_WEATHER_API_KEY;
    #endif

    ESP_LOGI(TAG, "HTTP request to get weather");
    ESP_LOGE(TAG,"URL: http://api.openweathermap.org%s",queryString.c_str());

    esp_http_client_config_t config = {
        .host = WEB_API_URL,
        .path = queryString.c_str(),
        .event_handler = http_event_handle,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
    ESP_LOGI(TAG, "Status = %d, content_length = %" PRId64 , // PRIu64  / PRIx64 to print in hexadecimal.
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));
    } else {
        return ESP_FAIL;
    }

    esp_http_client_cleanup(client);    
    //ESP_LOGE(TAG,"JSON:\n%s",local_response_buffer);
    jsonString = local_response_buffer;
    return ESP_OK;

}