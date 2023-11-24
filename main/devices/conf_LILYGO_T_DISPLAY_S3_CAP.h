#pragma once

#undef SD_SUPPORTED

#define PIN_LCD_BL 38

#define PIN_LCD_D0 39
#define PIN_LCD_D1 40
#define PIN_LCD_D2 41
#define PIN_LCD_D3 42
#define PIN_LCD_D4 45
#define PIN_LCD_D5 46
#define PIN_LCD_D6 47
#define PIN_LCD_D7 48

#define PIN_POWER_ON 15

#define PIN_LCD_RES 5
#define PIN_LCD_CS 6
#define PIN_LCD_DC 7
#define PIN_LCD_WR 8
#define PIN_LCD_RD 9

#define PIN_BUTTON_1 0
#define PIN_BUTTON_2 14
#define PIN_BAT_VOLT 4

#define PIN_IIC_SCL 17
#define PIN_IIC_SDA 18

#define PIN_TOUCH_INT 16
#define PIN_TOUCH_RES 21

// // Portrait
#define TFT_WIDTH 170
#define TFT_HEIGHT 320

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_Parallel8 _bus_instance; // 8ビットパラレルバスのインスタンス (ESP32のみ)
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_CST816S _touch_instance;

public:
    // コンストラクタを作成し、ここで各種設定を行います。
    // クラス名を変更した場合はコンストラクタも同じ名前を指定してください。
    LGFX(void)
    {
        {                                      // バス制御の設定を行います。
            auto cfg = _bus_instance.config(); // バス設定用の構造体を取得します。

            // 8ビットパラレルバスの設定
            // cfg.i2s_port = I2S_NUM_0;  // 使用するI2Sポートを選択 (I2S_NUM_0 or I2S_NUM_1) (ESP32のI2S
            // LCDモードを使用します)
            cfg.freq_write = 20000000; // 送信クロック (最大20MHz, 80MHzを整数で割った値に丸められます)
            cfg.pin_wr = PIN_LCD_WR;   // WR を接続しているピン番号
            cfg.pin_rd = PIN_LCD_RD;   // RD を接続しているピン番号
            cfg.pin_rs = PIN_LCD_DC;   // RS(D/C)を接続しているピン番号
            cfg.pin_d0 = PIN_LCD_D0;   // D0を接続しているピン番号
            cfg.pin_d1 = PIN_LCD_D1;   // D1を接続しているピン番号
            cfg.pin_d2 = PIN_LCD_D2;   // D2を接続しているピン番号
            cfg.pin_d3 = PIN_LCD_D3;   // D3を接続しているピン番号
            cfg.pin_d4 = PIN_LCD_D4;   // D4を接続しているピン番号
            cfg.pin_d5 = PIN_LCD_D5;   // D5を接続しているピン番号
            cfg.pin_d6 = PIN_LCD_D6;   // D6を接続しているピン番号
            cfg.pin_d7 = PIN_LCD_D7;   // D7を接続しているピン番号

            _bus_instance.config(cfg);              // 設定値をバスに反映します。
            _panel_instance.setBus(&_bus_instance); // バスをパネルにセットします。
        }

        {                                        // 表示パネル制御の設定を行います。
            auto cfg = _panel_instance.config(); // 表示パネル設定用の構造体を取得します。

            cfg.pin_cs = PIN_LCD_CS;   // CSが接続されているピン番号   (-1 = disable)
            cfg.pin_rst = PIN_LCD_RES; // RSTが接続されているピン番号  (-1 = disable)
            cfg.pin_busy = -1;         // BUSYが接続されているピン番号 (-1 = disable)

            // ※
            // 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。

            cfg.panel_width = 170;    // 実際に表示可能な幅
            cfg.panel_height = 320;   // 実際に表示可能な高さ
            cfg.offset_x = 35;        // パネルのX方向オフセット量
            cfg.offset_y = 0;         // パネルのY方向オフセット量
            cfg.offset_rotation = 0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
            cfg.dummy_read_pixel = 8; // ピクセル読出し前のダミーリードのビット数
            cfg.dummy_read_bits = 1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
            cfg.readable = true;      // データ読出しが可能な場合 trueに設定
            cfg.invert = true;        // パネルの明暗が反転してしまう場合 trueに設定
            cfg.rgb_order = false;    // パネルの赤と青が入れ替わってしまう場合 trueに設定
            cfg.dlen_16bit = false;   // 16bitパラレルやSPIでデータ長を16bit単位で送信するパネルの場合 trueに設定
            cfg.bus_shared = true;    // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

            _panel_instance.config(cfg);
        }

        {                                        // バックライト制御の設定を行います。（必要なければ削除）
            auto cfg = _light_instance.config(); // バックライト設定用の構造体を取得します。

            cfg.pin_bl = 38;     // バックライトが接続されているピン番号
            cfg.invert = false;  // バックライトの輝度を反転させる場合 true
            cfg.freq = 44100;    // バックライトのPWM周波数
            cfg.pwm_channel = 7; // 使用するPWMのチャンネル番号

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // バックライトをパネルにセットします。
        }
        {
            auto cfg = _touch_instance.config();
            cfg.offset_rotation = 0;

            cfg.x_min = 0;
            cfg.x_max = 320;
            cfg.y_min = 0;
            cfg.y_max = 170;

            // I2C接続
            cfg.i2c_port = 0;
            cfg.pin_sda = 18;
            cfg.pin_scl = 17;
            cfg.pin_int = 16;
            cfg.pin_rst = 21;

            cfg.freq = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance); // 使用するパネルをセットします。
    }
};