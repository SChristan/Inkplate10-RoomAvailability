#ifndef ARDUINO_INKPLATE10
#error "Wrong board selected, please select Inkplate 10 in the boards menu."
#endif

#include <Inkplate.h>
#include <ArduinoJson.h>
#include "my_wifi.h"
#include "webuntis.h"
#include "timetable.h"
#include "wlan_credentials.h"

#define TIME_TO_RELEASE_TOUCHPAD 1  // Time ESP32 will go to sleep before resetting to current week after Touchpad action (in seconds)

Inkplate display(INKPLATE_1BIT);

RTC_DATA_ATTR int8_t week_offset = 0;

void setup() {
  Serial.begin(115200);
  display.begin();

  prepareDisplayPins();

  time_t timetable_time = display.rtcGetEpoch() + week_offset * time_t(7 * 24 * 60 * 60);
  bool today_is_set = true;
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    // Check the touchpads 10 times with a short delay in beetween
    bool touchpad_pressed = false;
    for (uint8_t i = 0; i < 200; i++) {
      // Check if the first touchpad has been touched.
      if (display.readTouchpad(PAD1)) {
        Serial.println("Pressed Touchpad 1!");
        week_offset--;
        timetable_time = timetable_time - time_t(7 * 24 * 60 * 60);
        touchpad_pressed = true;
        today_is_set = false;
      }
      // Check if the second touchpad has been touched.
      else if (display.readTouchpad(PAD2)) {
        Serial.println("Pressed Touchpad 2!");
        week_offset = 0;
        timetable_time = display.rtcGetEpoch();
        touchpad_pressed = true;
        today_is_set = true;
      }
      // Check if the third touchpad has been touched.
      else if (display.readTouchpad(PAD3)) {
        Serial.println("Pressed Touchpad 3!");
        week_offset++;
        timetable_time = timetable_time + time_t(7 * 24 * 60 * 60);
        touchpad_pressed = true;
        today_is_set = false;
      }
      // If a touchpad has been pressed, wait a short time for the user to release the touchpad.
      if (touchpad_pressed) {
        performUpdate(timetable_time);
        Serial.println("Display was refreshed, time to release touchpads...");
        delay(TIME_TO_RELEASE_TOUCHPAD * 1000);
        // Clear the touchpads
        display.readTouchpad(PAD1);
        display.readTouchpad(PAD2);
        display.readTouchpad(PAD3);
        touchpad_pressed = false;
      } else {
        delay(300);
      }
    }
  } else {
    Serial.println("Auto reset to today!");
    timetable_time = display.rtcGetEpoch();
    week_offset = 0;
    performUpdate(timetable_time);
  }

  if (today_is_set) {
    uint64_t hour_difference = hoursUntilWakeUp(1);
    // Go to deep sleep for the specified period of time
    esp_sleep_enable_timer_wakeup(hour_difference * 60 * 60 * 1000 * 1000);
    Serial.println("Going to sleep for " + String(hour_difference) + " hours...");
  } else {
    // Go to deep sleep for 1 minute
    esp_sleep_enable_timer_wakeup(60 * 1000 * 1000);
    Serial.println("Going to sleep for 1 minute...");
  }

  // Enable wakup from deep sleep on GPIO 34, which are the touchpads
  esp_sleep_enable_ext1_wakeup(int64_t(1) << GPIO_NUM_34, ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_deep_sleep_start();
}

void loop() {
  // no code, because of low power mode
}

uint8_t hoursUntilWakeUp(uint8_t required_hour) {
  display.rtcGetRtcData();
  if (display.rtcGetHour() == required_hour) {
    return 24;
  } else {
    int8_t delta_real = required_hour - display.rtcGetHour();
    uint8_t delta_in_hours = (delta_real + 24) % 24;
    return delta_in_hours;
  }
}

void prepareDisplayPins() {
  display.setIntOutput(1, false, false, HIGH, IO_INT_ADDR);
  display.setIntPin(PAD1, RISING, IO_INT_ADDR);
  display.setIntPin(PAD2, RISING, IO_INT_ADDR);
  display.setIntPin(PAD3, RISING, IO_INT_ADDR);
}

void performUpdate(time_t time_epoch) {
  static bool wifi_is_connected = false;
  if (!wifi_is_connected) {
    display.clearDisplay();
    MyWiFi wifi(display, WLAN_SSID_WPA2, WLAN_IDENTITY_WPA2, WLAN_USERNAME_WPA2, WLAN_PASSWORD_WPA2);
    //MyWiFi wifi(display, WLAN_SSID, WLAN_PASSWORD);
    wifi.connect();
    wifi_is_connected = true;
  }
  display.clearDisplay();

  Timetable timetable(display, 460, time_epoch);  //  S-132 = 663 | S101 = 457 | S103 = 460
  timetable.drawBackground();
  timetable.drawTimesAndDays();
  bool data_loaded_successfully = timetable.drawData();
  if (!data_loaded_successfully) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(5, 5);
    display.println("Der Raumbelegungsplan konnte nicht geladen werden, bitte versuchen Sie es erneut.");
  }
  display.display();
}
