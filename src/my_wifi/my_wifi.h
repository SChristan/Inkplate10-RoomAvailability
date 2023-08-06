/**********************************************************
* Include Guard
**********************************************************/
#ifndef MY_WIFI_H_
#define MY_WIFI_H_

/**********************************************************
* Private Includes
**********************************************************/
#include <Inkplate.h>

/*********************************************************/

class MyWiFi {
/**********************************************************
* Public Function Prototypes
**********************************************************/
public:
  MyWiFi(Inkplate& display, char* wlan_ssid, char* wlan_identity, char* wlan_username, char* wlan_password);
  MyWiFi(Inkplate& display, char* wlan_ssid, char* wlan_password);
  void connect();
  void searchNetworks();

/**********************************************************
* Private Member Variables
**********************************************************/
private:
  Inkplate& display_;
  bool wlan_with_wpa2_;
  char* wlan_ssid_;
  char* wlan_identity_;
  char* wlan_username_;
  char* wlan_password_;
  uint8_t text_size_;
};

#endif
