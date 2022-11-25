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
    Fontawesome Weather Symbols available in font_fa_weather_xx.c
    https://openweathermap.org/weather-conditions#Weather-Condition-Codes-2
*/

//LV_FONT_DECLARE(font_fa_weather_32)
LV_FONT_DECLARE(font_fa_weather_42)
//LV_FONT_DECLARE(font_fa_weather_48)
//LV_FONT_DECLARE(font_fa_weather_56)

#define FA_WEATHER_BOLT                 "\xEF\x83\xA7"      // f0e7
#define FA_WEATHER_SUN                  "\xEF\x86\x85"      // f185
#define FA_WEATHER_WIND                 "\xEF\x9C\xAE"      // f72e

#define FA_WEATHER_SMOG                 "\xEF\x9D\x9F"      // f75f

// Group 2xx: Thunderstorm - 200-232
#define FA_WEATHER_CLOUD_BOLT           "\xEF\x9D\xAC"      // f76c
#define FA_WEATHER_CLOUD_SHOWERS_HEAVY  "\xEF\x9D\x80"      // f740

// Group 3xx: Drizzle - 300-321


// Group 5xx: Rain - 500-531
#define FA_WEATHER_CLOUD_SHOWERS_WATER  "\xEE\x93\xA4"      // e4e4
#define FA_WEATHER_CLOUD_RAIN           "\xEF\x9C\xBD"      // f73d
#define FA_WEATHER_CLOUD_MOON_RAIN      "\xEF\x9C\xBC"      // f73c

#define FA_WEATHER_CLOUD_SUN_RAIN       "\xEF\x9D\x83"      // f743

// Group 7xx: Atmosphere - 701-781
#define FA_WEATHER_TORNADO              "\xEF\x9D\xAF"      // f76f // owm 781
#define FA_WEATHER_VOLCANO              "\xEF\x9D\xB0"      // f770 // owm 762
#define FA_WEATHER_DROPLET              "\xEF\x81\x83"      // f043 // owm 701 (mist)

// Group 6xx: Snow - 600-622
#define FA_WEATHER_SNOWFLAKES           "\xEF\x8B\x9C"      // f2dc

#define FA_WEATHER_WATER                "\xEF\x9D\xB3"      // f773
#define FA_WEATHER_MOON                 "\xEF\x86\x86"      // f186

#define FA_WEATHER_HURRICANE            "\xEF\x9D\x91"      // f751

// Group 80x: Clouds 801-804
#define FA_WEATHER_CLOUD                "\xEF\x83\x82"      // f0c2
#define FA_WEATHER_CLOUD_SUN            "\xEF\x9B\x84"      // f6c4
#define FA_WEATHER_CLOUD_MOON           "\xEF\x9B\x83"      // f6c3



