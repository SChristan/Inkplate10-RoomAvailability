#ifndef WEBUNTIS_H_
#define WEBUNTIS_H_

#include <Inkplate.h>
#include <ArduinoJson.h>
#include <stdint.h>

class WebUntis {
public:
  WebUntis(Inkplate& display, uint16_t room_id);
  DynamicJsonDocument getData(uint16_t date_year, uint8_t date_month, uint8_t date_day);

private:
  Inkplate& display_;
  uint16_t room_id_;
};

#endif
