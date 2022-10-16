
#define SPI_MODE 3

#define TFT_MOSI 35 // SDA
#define TFT_MISO -1
#define TFT_SCLK 36 // SCL
#define TFT_DC 34
#define TFT_CS -1
#define TFT_RST 7
#define TFT_BCK_LT 6

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

class LGFX : public lgfx::LGFX_Device
{
  // provide an instance that matches the type of panel you want to connect to.
  lgfx::Panel_ST7789 _panel_instance;

  // provide an instance that matches the type of bus to which the panel is connected.
  lgfx::Bus_SPI _bus_instance; // Instances of spi buses

  lgfx::Light_PWM _light_instance;

public:
  // create a constructor and make various settings here.
  // if you change the class name, the constructor must also have the same name.

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = SPI_MODE;
      cfg.freq_write = 40000000;
      cfg.freq_read = 16000000;
      cfg.spi_3wire = true;
      cfg.use_lock = true;

      cfg.dma_channel = SPI_DMA_CH_AUTO;

      cfg.pin_sclk = TFT_SCLK;
      cfg.pin_mosi = TFT_MOSI;

      cfg.pin_miso = TFT_MISO;
      cfg.pin_dc = TFT_DC;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();

      cfg.pin_cs = TFT_CS;
      cfg.pin_rst = TFT_RST;
      cfg.pin_busy = -1;

      cfg.panel_width = TFT_WIDTH;   // actual visible width
      cfg.panel_height = TFT_HEIGHT; // actually visible height
      cfg.offset_x = 0;              // Panel X-direction offset amount
      cfg.offset_y = 0;              // Panel Y offset amount
      cfg.offset_rotation = 0;       // offset of rotational values from 0 to 7 (4 to 7 upside down)
      cfg.dummy_read_pixel = 8;      // number of bits in dummy leads before pixel read
      cfg.dummy_read_bits = 1;       // number of bits in dummy leads before reading non-pixel data
      cfg.readable = true;           // set to true if data can be read
      cfg.invert = true;             // set to true if the light and dark of the panel is reversed
      cfg.rgb_order = false;         // set to true if the red and blue of the panel are swapped
      cfg.dlen_16bit = false;        // Set to true for panels that transmit data lengths in 16-bit increments in 16-bit parallel or SPI
      cfg.bus_shared = true;         // Set to true when sharing the bus with sd card (bus control is performed with drawJpgFile, etc.)

      // The following should only be set if the display is misalized by a driver with a variable number of pixels, such as st7735 or ILI9163.
      // cfg.memory_width = 240; //Maximum width supported by driver ICs
      // cfg.memory_height = 320; //Maximum height supported by driver ICs

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = TFT_BCK_LT;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance); // Set the panel to be used.
  }
};
