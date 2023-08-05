/**********************************************************
* Private Includes
**********************************************************/
#include "my_wifi.h"
#include <WiFi.h>
#include <esp_wpa2.h>
#include <stdint.h>

/**********************************************************
* Class Constructors
**********************************************************/
MyWiFi::MyWiFi(Inkplate& display, char* wlan_ssid, char* wlan_identity, char* wlan_username, char* wlan_password)
  : display_{ display },
    wlan_with_wpa2_{ true },
    wlan_ssid_{ wlan_ssid },
    wlan_identity_{ wlan_identity },
    wlan_username_{ wlan_username },
    wlan_password_{ wlan_password },
    text_size_{ 3 } {
}

MyWiFi::MyWiFi(Inkplate& display, char* wlan_ssid, char* wlan_password)
  : display_{ display },
    wlan_with_wpa2_{ false },
    wlan_ssid_{ wlan_ssid },
    wlan_identity_{ },
    wlan_username_{ },
    wlan_password_{ wlan_password },
    text_size_{ 3 } {
}

/*********************************************************/

/**  @brief Connecting to the specified WiFi network.
 */
void MyWiFi::connect() {
  display_.setTextSize(text_size_);
  display_.setCursor(0, 0);
  display_.print("Connecting to WiFi...");
  display_.display();

  WiFi.mode(WIFI_MODE_STA);
  
  if (wlan_with_wpa2_) {
    WiFi.begin(wlan_ssid_, WPA2_AUTH_PEAP, wlan_identity_, wlan_username_, wlan_password_);
  } else {
    WiFi.begin(wlan_ssid_, wlan_password_);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display_.print('.');
    display_.partialUpdate();
  }
  display_.print("\nConnected to " + String(wlan_ssid_) + " with IP: " + WiFi.localIP().toString());
  display_.partialUpdate();
}

/*********************************************************/

/** @brief Searching for WiFi networks and printing them on the display.
 */
void MyWiFi::searchNetworks() {
  display_.setTextSize(text_size_);
  display_.setCursor(0, 0);
  display_.println("Scanning for WiFi networks...");
  display_.display();

  uint16_t n = WiFi.scanNetworks();
  if (n == 0) {
    display_.println("No WiFi networks found!");
    display_.partialUpdate();
  } else {
    for (uint16_t i = 0; i < 10; i++) {
      display_.print(WiFi.SSID(i));
      display_.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? 'O' : '*');
      display_.print(WiFi.RSSI(i), DEC);
      display_.print('\n');
    }
    display_.partialUpdate();
  }
}
