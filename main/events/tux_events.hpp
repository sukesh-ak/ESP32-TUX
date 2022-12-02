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


#ifndef TUX_EVENT_SOURCE_H_
#define TUX_EVENT_SOURCE_H_

#include "esp_event.h"
//#include "esp_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

// Declare an event base
ESP_EVENT_DECLARE_BASE(TUX_EVENTS);        // declaration of the TUX_EVENTS family

// declaration of the specific events under the TUX_EVENTS family
enum {                                       
    TUX_EVENT_DATETIME_SET,                  // Date updated through SNTP 

    TUX_EVENT_OTA_STARTED,                   // Invoke OTA START
    TUX_EVENT_OTA_IN_PROGRESS,               // OTA Progress including %
    TUX_EVENT_OTA_ROLLBACK,                  // OTA Rollback
    TUX_EVENT_OTA_COMPLETED,                 // OTA Completed
    TUX_EVENT_OTA_FAILED,                    // OTA Failed
    TUX_EVENT_OTA_ABORTED,                   // OTA Aborted

    TUX_EVENT_WEATHER_UPDATED,               // Weather updated
    TUX_EVENT_THEME_CHANGED                  // raised when the theme changes
};

#ifdef __cplusplus
}
#endif

#endif // #ifndef TUX_EVENT_SOURCE_H_