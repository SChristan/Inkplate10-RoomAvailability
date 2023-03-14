#ifndef MY_WIFI_H_
#define MY_WIFI_H_

#include <Inkplate.h>

class MyWiFi {
public:
  MyWiFi(Inkplate& display, char* wlan_ssid, char* wlan_password);
  void connect();
  void searchNetworks();

private:
  Inkplate& display_;
  char* wlan_ssid_;
  char* wlan_password_;
  uint8_t text_size_;
};

#endif
