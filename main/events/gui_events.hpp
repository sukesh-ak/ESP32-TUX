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
    Used to communicate events from code to LVGL GUI
    lv_msg event codes and related variables
*/
#ifndef GUI_EVENT_DEF_H_
#define GUI_EVENT_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MSG_PAGE_HOME           1
#define MSG_PAGE_REMOTE         2
#define MSG_PAGE_SETTINGS       3
#define MSG_PAGE_OTA            4

// Used for WiFI status update
#define MSG_WIFI_PROV_MODE      50
#define MSG_WIFI_CONNECTED      51
#define MSG_WIFI_DISCONNECTED   52

// Updates during OTA
#define MSG_OTA_STATUS          55
#define MSG_OTA_INITIATE        56

#define MSG_SDCARD_STATUS       57
#define MSG_BATTERY_STATUS      58

#define MSG_TIME_CHANGED        100
#define MSG_WEATHER_CHANGED     101

#ifdef __cplusplus
}
#endif

#endif // #ifndef GUI_EVENT_DEF_H_
