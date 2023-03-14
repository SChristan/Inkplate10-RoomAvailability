#ifndef ARDUINO_INKPLATE10
#error "Wrong board selected, please select Inkplate 10 in the boards menu."
#endif

#include <Inkplate.h>
#include <ArduinoJson.h>
#include "my_wifi.h"
#include "webuntis.h"
#include "timetable.h"
#include "wlan_credentials.h"

#define TIME_TO_SLEEP_SHORT 30      // Time ESP32 will go to sleep before resetting to current week after Touchpad action (in seconds)
#define TIME_TO_RELEASE_TOUCHPAD 3  // Time ESP32 will go to sleep before resetting to current week after Touchpad action (in seconds)

Inkplate display(INKPLATE_1BIT);

RTC_DATA_ATTR int bootCount = 0;

void setup() {
  Serial.begin(115200);
  display.begin();
  display.setRotation(2);

  prepareDisplayPins();
  ++bootCount;
  displayInfo();
  display.clearDisplay();
  performUpdate();

  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  bool today_is_set = true;

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    // Check the touchpads 10 times with a short delay in beetween
    bool touchpad_pressed = false;
    for (uint8_t i = 0; i < 100; i++) {
      // Check if the first touchpad has been touched.
      if (display.readTouchpad(PAD1)) {
        Serial.println("Pressed Touchpad 1!");
        delay(2000);  // time to do some tasks which need 500ms
        touchpad_pressed = true;
        today_is_set = false;
      }
      // Check if the second touchpad has been touched.
      else if (display.readTouchpad(PAD2)) {
        Serial.println("Pressed Touchpad 2!");
        delay(2000);  // time to do some tasks which need 500ms
        touchpad_pressed = true;
        today_is_set = true;
      }
      // Check if the third touchpad has been touched.
      else if (display.readTouchpad(PAD3)) {
        Serial.println("Pressed Touchpad 3!");
        delay(2000);  // time to do some tasks which need 500ms
        touchpad_pressed = true;
        today_is_set = false;
      }
      // If a touchpad has been pressed, wait a short time for the user to release the touchpad.
      if (touchpad_pressed) {
        Serial.println("Display was refreshed, taking a break...");
        delay(TIME_TO_RELEASE_TOUCHPAD * 1000);
        // Clear the touchpads
        display.readTouchpad(PAD1);
        display.readTouchpad(PAD2);
        display.readTouchpad(PAD3);
        touchpad_pressed = false;
        //break;
      } else {
        delay(300);
      }
    }
  } else {
    Serial.println("Pressed Touchpad 2! -- today");
    delay(2000);  // time to do some tasks which need 500ms
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

void performUpdate() {
  MyWiFi wifi(display, WLAN_SSID, WLAN_PASSWORD);
  wifi.connect();
  display.clearDisplay();

  display.setCursor(600, 5);
  display.rtcGetRtcData();
  display.print(String(display.rtcGetHour()) + ":" + String(display.rtcGetMinute()) + ":" + String(display.rtcGetSecond()) + "  -  " + String(display.rtcGetEpoch()));
  Serial.println("RTC Epoch: " + String(display.rtcGetEpoch()));
  display.display();

  Timetable timetable(display, 457, 1668423349 + 3600 * 24);  //display_.rtcGetEpoch()); // 457 663
  timetable.drawBackground();
  timetable.drawTimesAndDays();
  bool data_loaded_successfully = timetable.drawData();
  if (data_loaded_successfully) {
    display.display();
  }
}

void displayInfo() {
  // First, lets delete everything from frame buffer
  display.clearDisplay();

  // Set text cursor and size
  display.setCursor(10, 280);
  display.setTextSize(2);

  display.print(F("Boot count: "));
  display.println(bootCount, DEC);  // Print the number

  // Set next line cursor position
  display.setCursor(10, 320);

  // Display wake up reason
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      display.println("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      display.println("Wakeup caused by external signal using RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      display.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      display.println("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      display.println("Wakeup caused by ULP program");
      break;
    default:
      display.println("Wakeup was not caused by deep sleep");
      break;
  }

  display.display();
}
