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

/* FOR NOW THIS BOARD IS NOT TESTED */
#define MAKERFAB_ESP32S3_16P
#define SD_SUPPORTED 

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// Portrait
#define TFT_WIDTH   320
#define TFT_HEIGHT  480

#define LCD_CS 37
#define LCD_BLK 45

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488     _panel_instance;
    lgfx::Bus_Parallel16    _bus_instance; 
    lgfx::Light_PWM         _light_instance;
    lgfx::Touch_FT5x06      _touch_instance;
    //lgfx::Touch_NS2009 _touch_instance;

public:
    LGFX(void)
    {
        {                                      
            auto cfg = _bus_instance.config(); 
            
            cfg.port = 0;              
            cfg.freq_write = 20000000; 
            cfg.pin_wr = 35;           
            cfg.pin_rd = 48;           
            cfg.pin_rs = 36;           

            cfg.pin_d0 = 47;
            cfg.pin_d1 = 21;
            cfg.pin_d2 = 14;
            cfg.pin_d3 = 13;
            cfg.pin_d4 = 12;
            cfg.pin_d5 = 11;
            cfg.pin_d6 = 10;
            cfg.pin_d7 = 9;
            cfg.pin_d8 = 3;
            cfg.pin_d9 = 8;
            cfg.pin_d10 = 16;
            cfg.pin_d11 = 15;
            cfg.pin_d12 = 7;
            cfg.pin_d13 = 6;
            cfg.pin_d14 = 5;
            cfg.pin_d15 = 4;

            _bus_instance.config(cfg);              
            _panel_instance.setBus(&_bus_instance); 
        }

        {                                        
            auto cfg = _panel_instance.config(); 

            cfg.pin_cs = -1;   
            cfg.pin_rst = -1;  
            cfg.pin_busy = -1; 

            cfg.memory_width = TFT_WIDTH;   
            cfg.memory_height = TFT_HEIGHT;  
            cfg.panel_width = TFT_WIDTH;    
            cfg.panel_height = TFT_HEIGHT;   
            cfg.offset_x = 0;         
            cfg.offset_y = 0;         
            cfg.offset_rotation = 0;  
            cfg.dummy_read_pixel = 8; 
            cfg.dummy_read_bits = 1;  
            cfg.readable = true;      
            cfg.invert = false;       
            cfg.rgb_order = false;    
            cfg.dlen_16bit = true;    
            cfg.bus_shared = true;    

            _panel_instance.config(cfg);
        }

    {
      auto cfg = _light_instance.config();    

      cfg.pin_bl = 45;              
      cfg.invert = false;           
      cfg.freq   = 44100;           
      cfg.pwm_channel = 7;          

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  
    }

    { 
      auto cfg = _touch_instance.config();

      cfg.x_min      = 0;
      cfg.x_max      = TFT_WIDTH-1;
      cfg.y_min      = 0;  
      cfg.y_max      = TFT_HEIGHT-1;
      cfg.pin_int    = 40;  
      cfg.bus_shared = true; 
      cfg.offset_rotation = 0;

      cfg.i2c_port = 0;     //I2C_NUM_1;
      cfg.i2c_addr = 0x38;  // NS2009 = 0x48;
      cfg.pin_sda  = 38;   
      cfg.pin_scl  = 39;   
      cfg.freq = 400000;  

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);  
    }
        setPanel(&_panel_instance); 
    }
};