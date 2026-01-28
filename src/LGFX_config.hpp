#ifndef LGFX_CONFIG_HPP
#define LGFX_CONFIG_HPP

#include "pins.h"
#include <LovyanGFX.hpp>

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();

      cfg.spi_host = 0;
      cfg.spi_mode = 0;
      cfg.freq_write = 80000000;
      cfg.freq_read =  20000000;
      cfg.pin_sclk = TFT_SCL;
      cfg.pin_mosi = TFT_SDA;
      cfg.pin_miso = -1;
      cfg.pin_dc = TFT_DC;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = TFT_CS;
      cfg.pin_rst = TFT_RST;
      cfg.pin_busy = -1;
      cfg.panel_width      =   SCREEN_WIDTH;
      cfg.panel_height     =   SCREEN_HEIGHT;
      cfg.offset_x         =     0;
      cfg.offset_y         =     0;
      cfg.readable         =  true;
      cfg.invert           =  true;

      _panel_instance.config(cfg);
    }
    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = TFT_BL;
      cfg.invert = false;
      cfg.freq   = 12000;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance);
  }
};

#endif
