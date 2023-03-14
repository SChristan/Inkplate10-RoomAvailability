#include "my_wifi.h"
#include <WiFi.h>
#include <stdint.h>

MyWiFi::MyWiFi(Inkplate& display, char* wlan_ssid, char* wlan_password)
  : display_{ display },
    wlan_ssid_{ wlan_ssid },
    wlan_password_{ wlan_password },
    text_size_{ 3 } {
}

void MyWiFi::connect() {
  display_.setTextSize(text_size_);
  display_.setCursor(0, 0);
  display_.print("Connecting to WiFi...");
  display_.display();

  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(wlan_ssid_, wlan_password_);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display_.print('.');
    display_.partialUpdate();
  }
  display_.println("\nConnected to " + String(wlan_ssid_) + "!");
  display_.partialUpdate();
}

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
