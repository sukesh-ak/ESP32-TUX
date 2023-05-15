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
  TFT + TOUCH & SD CARD WORKING
*/

#define ILI9341_SD
#define SD_SUPPORTED 

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <driver/i2c.h>

// SD CARD - SPI
#define SDSPI_HOST_ID SPI3_HOST
#define LCD_MISO       GPIO_NUM_19
#define LCD_MOSI       GPIO_NUM_23
#define LCD_SCLK       GPIO_NUM_18
#define LCD_CS         GPIO_NUM_5
#define LCD_DC         GPIO_NUM_4
#define LCD_RST         GPIO_NUM_22
#define LCD_BL         GPIO_NUM_15

#define TOUCH_CS         GPIO_NUM_14
#define TOUCH_IR         GPIO_NUM_27

#define SD_CS         GPIO_NUM_21
#define SD_MOSI         LCD_MOSI
#define SD_SCLK         LCD_SCLK
#define SD_MISO         LCD_MISO

// Portrait
#define TFT_WIDTH   240
#define TFT_HEIGHT  320








class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ILI9341     _panel_instance;

  lgfx::Bus_SPI        _bus_instance;   // SPIバスのインスタンス

  lgfx::Light_PWM     _light_instance;

  lgfx::Touch_XPT2046           _touch_instance; // FT5206, FT5306, FT5406, FT6206, FT6236, FT6336, FT6436


public:


  LGFX(void)
  {
    { 
      auto cfg = _bus_instance.config();   


      cfg.spi_host = VSPI_HOST;     // 使用するSPIを選択  ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
    
      cfg.spi_mode = 0;             // SPI通信モードを設定 (0 ~ 3)
      cfg.freq_write = 70000000;    // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
      cfg.freq_read  = 48000000;    // 受信時のSPIクロック
      cfg.spi_3wire  = true;        // 受信をMOSIピンで行う場合はtrueを設定
      cfg.use_lock   = true;        // トランザクションロックを使用する場合はtrueを設定
      cfg.dma_channel = SPI_DMA_CH_AUTO; // 使用するDMAチャンネルを設定 (0=DMA不使用 / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=自動設定)
      // ※ ESP-IDFバージョンアップに伴い、DMAチャンネルはSPI_DMA_CH_AUTO(自動設定)が推奨になりました。1ch,2chの指定は非推奨になります。
      cfg.pin_sclk = LCD_SCLK;            // SPIのSCLKピン番号を設定
      cfg.pin_mosi = LCD_MOSI;            // SPIのMOSIピン番号を設定
      cfg.pin_miso = LCD_MISO;            // SPIのMISOピン番号を設定 (-1 = disable)
      cfg.pin_dc   = LCD_DC;            // SPIのD/Cピン番号を設定  (-1 = disable)

      _bus_instance.config(cfg);    // 設定値をバスに反映します。
      _panel_instance.setBus(&_bus_instance);      // バスをパネルにセットします。
    }

    { // 表示パネル制御の設定を行います。
      auto cfg = _panel_instance.config();    // 表示パネル設定用の構造体を取得します。

      cfg.pin_cs           =    5;  // CSが接続されているピン番号   (-1 = disable)
      cfg.pin_rst          =    22;  // RSTが接続されているピン番号  (-1 = disable)
      cfg.pin_busy         =    -1;  // BUSYが接続されているピン番号 (-1 = disable)
// 3628
// 481
// 3808
// 3885
// 373
// 436
// 491
     
      cfg.panel_width      =   240;  // 実際に表示可能な幅
      cfg.panel_height     =   320;  // 実際に表示可能な高さ
      cfg.offset_x         =     0;  // パネルのX方向オフセット量
      cfg.offset_y         =     0;  // パネルのY方向オフセット量
      cfg.offset_rotation  =     0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
      cfg.dummy_read_pixel =     8;  // ピクセル読出し前のダミーリードのビット数
      cfg.dummy_read_bits  =     1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
      cfg.readable         =  true;  // データ読出しが可能な場合 trueに設定
      cfg.invert           = false;  // パネルの明暗が反転してしまう場合 trueに設定
      cfg.rgb_order        = false;  // パネルの赤と青が入れ替わってしまう場合 trueに設定
      cfg.dlen_16bit       = false;  // 16bitパラレルやSPIでデータ長を16bit単位で送信するパネルの場合 trueに設定
      cfg.bus_shared       =  true;  // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)



      _panel_instance.config(cfg);
    }

    

    { // バックライト制御の設定を行います。（必要なければ削除）
      auto cfg = _light_instance.config();    // バックライト設定用の構造体を取得します。

      cfg.pin_bl = LCD_BL;              // バックライトが接続されているピン番号
      cfg.invert = false;           // バックライトの輝度を反転させる場合 true
      cfg.freq   = 44100;           // バックライトのPWM周波数
      cfg.pwm_channel = 7;          // 使用するPWMのチャンネル番号

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // バックライトをパネルにセットします。
    }


    { // タッチスクリーン制御の設定を行います。（必要なければ削除）
      auto cfg = _touch_instance.config();
 //uint16_t calData[5] = { 377, 3278, 458, 3421, 2 };
      cfg.x_min      = 377;    // タッチスクリーンから得られる最小のX値(生の値)
      cfg.x_max      = 3278;  // タッチスクリーンから得られる最大のX値(生の値)
      cfg.y_min      = 458;    // タッチスクリーンから得られる最小のY値(生の値)
      cfg.y_max      = 3421;  // タッチスクリーンから得られる最大のY値(生の値)
      //cfg.pin_int    = 38;   // INTが接続されているピン番号
      cfg.bus_shared = true; // 画面と共通のバスを使用している場合 trueを設定
      cfg.offset_rotation = 0;// 表示とタッチの向きのが一致しない場合の調整 0~7の値で設定

// SPI接続の場合
      cfg.spi_host = VSPI_HOST;// 使用するSPIを選択 (HSPI_HOST or VSPI_HOST)
      cfg.freq = 1000000;     // SPIクロックを設定
      cfg.pin_sclk = 18;     // SCLKが接続されているピン番号
      cfg.pin_mosi = 23;     // MOSIが接続されているピン番号
      cfg.pin_miso = 19;     // MISOが接続されているピン番号
      cfg.pin_cs   =  14;     //   CSが接続されているピン番号
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);  // タッチスクリーンをパネルにセットします。
    }


    setPanel(&_panel_instance); // 使用するパネルをセットします。
  }
};

