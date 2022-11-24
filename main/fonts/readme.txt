 
*** Create Custom symbols ***

Download FontAwesome Free version or 
take from lvgl\scripts\built_in_font\FontAwesome5-Solid+Brands+Regular.woff
Automation done with this => https://github.com/lvgl/lv_font_conv

Goto conversion tool
https://lvgl.io/tools/fontconverter

Name eg. font_fa_14
Size : 14
Bpp : 4 bit-per-pixel
Symbols: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0987654321: *.,-[]()/'

Select 'fa-brands-400.ttf' from download
Range: Provide the symbol hex values with comma separated

Use this site to convert Unicode to Hex UTF-8 bytes
https://www.cogsci.ed.ac.uk/~richard/utf-8.cgi?input=f294&mode=hex

then define constants for us like
#define FA_BLE_SYMBOL "\xEF\x8A\x94"

For 7seg font
0123456789 .:'

/*
    Free PNG icons => https://www.flaticon.com/search?word=charte&shape=outline&order_by=4
*/
For images conversion, pick CF_TRUE_COLOR_ALPHA and enable dither

/*
    FA_WEATHER
    Data om the Range field
*/

https://fontawesome.com/icons/cloud?s=solid&f=classic
cloud = f0c2
bolt = f0e7
sun f185
wind f72e
tornado f76f
temperature-half f2c9
smog f75f
cloud-bolt f76c
bolt-lightning e0b7
snowflakes f2dc
poo-storm f75a
water f773
moon f186
volcano f770
temperature-three-quarters f2c8
temperature-quarter f2ca
temperature-low f76b
temperature-high f769
temperature-full f2c7
temperature-empty f2cb
temperature-arrow-up e040
temperature-arrow-down e03f
sun-plant-wilt e57a
rainbow f75b
hurricane f751
house-tsunami e515
cloud-sun-rain = f743
cloud-sun = f6c4
cloud-showers-water e4e4
cloud-showers-heavy f740
cloud-rain = f73d
cloud-moon-rain f73c
cloud-moon f6c3
droplet f043
Font Awesome 6 Free-Solid-900.otf
0xf0c2,0xf0e7,0xf185,0xf72e,0xf76f,0xf2c9,0xf75f,0xf76c,0xe0b7,0xf2dc,0xf75a,0xf773,0xf186,0xf770,0xf2c8,0xf2ca,0xf76b,0xf769,0xf2c7,0xf2cb,0xe040,0xe03f,0xe57a,0xf75b,0xf751,0xe515,0xf743,0xf6c4,0xe4e4,0xf740,0xf73d,0xf73c,0xf6c3,0xf75c
