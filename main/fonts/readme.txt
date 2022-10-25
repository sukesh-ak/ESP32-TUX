 
*** Create Custom symbols ***

Download FontAwesome Free version or 
take from lvgl\scripts\built_in_font\FontAwesome5-Solid+Brands+Regular.woff
Automation done with this => https://github.com/lvgl/lv_font_conv

Goto conversion tool
https://lvgl.io/tools/fontconverter

Name eg. font_fa_14
Size : 14
Bpp : 4 bit-per-pixel

Select 'fa-brands-400.ttf' from download
Range: Provide the symbol hex values with comma separated

Use this site to convert Unicode to Hex UTF-8 bytes
https://www.cogsci.ed.ac.uk/~richard/utf-8.cgi?input=f294&mode=hex

then define constants for us like
#define FA_BLE_SYMBOL "\xEF\x8A\x94"


/*
    Free PNG icons => https://www.flaticon.com/search?word=charte&shape=outline&order_by=4
*/
For images conversion, pick CF_TRUE_COLOR_ALPHA and enable dither