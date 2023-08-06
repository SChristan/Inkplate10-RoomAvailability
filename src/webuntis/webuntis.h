/**********************************************************
* Include Guard
**********************************************************/
#ifndef WEBUNTIS_H_
#define WEBUNTIS_H_

/**********************************************************
* Private Includes
**********************************************************/
#include <Inkplate.h>
#include <ArduinoJson.h>
#include <stdint.h>

/*********************************************************/

class WebUntis {
/**********************************************************
* Public Function Prototypes
**********************************************************/
public:
  WebUntis(Inkplate& display, uint16_t room_id);
  DynamicJsonDocument getData(uint16_t date_year, uint8_t date_month, uint8_t date_day);

/**********************************************************
* Private Member Variables
**********************************************************/
private:
  Inkplate& display_;
  uint16_t room_id_;
};

#endif
