#include "webuntis.h"
#include <HTTPClient.h>

WebUntis::WebUntis(Inkplate& display, uint16_t room_id)
  : display_{ display },
    room_id_{ room_id } {
}

DynamicJsonDocument WebUntis::getData(uint16_t date_year, uint8_t date_month, uint8_t date_day) {
  DynamicJsonDocument doc(65536);
  HTTPClient http;

  http.useHTTP10(true);

  if (http.begin("https://kephiso.webuntis.com/WebUntis/api/public/timetable/weekly/data?school=OTH-Regensburg&elementType=4&elementId=" + String(room_id_) + "&date=" + String(date_year) + "-" + String(date_month) + "-" + String(date_day))) {
    if (http.GET() > 0) {
      DeserializationError err = deserializeJson(doc, http.getStream());
      if (err) {
        deserializeJson(doc, "{}");
      }
    }
  } else {
    deserializeJson(doc, "{}");
  }
  return doc;
}
