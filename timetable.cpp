#include "timetable.h"
#include "oth_logo.h"
#include "webuntis.h"
#include <string.h>

Timetable::Timetable(Inkplate& display, uint16_t room_id, time_t time_epoch)
  : display_{ display },
    room_id_{ room_id },

    text_size_{ 2 },
    text_size_headline_{ 4 },
    pixel_width_letter_{ 6 },
    pixel_height_letter_{ 8 },

    display_margin_{ 5 },
    cell_margin_{ 5 },
    line_thickness_{ 1 },
    row_height_head_{ 50 },
    row_height_{ 85 },
    column_width_head_{ 170 },
    column_width_{ (display_.width() - 2 * display_margin_ - column_width_head_) / day_number_ },

    times_{ "08:15 - 09:45", "10:00 - 11:30", "11:45 - 13:15", "13:45 - 15:15", "15:30 - 17:00", "17:15 - 18:45", "19:00 - 20:30" },
    days_{ "Mo.", "Di.", "Mi.", "Do.", "Fr.", "Sa." },
    lesson_number_{ 0 },

    time_epoch_{ time_epoch },
    time_info_{ localtime(&time_epoch_) },
    time_epoch_monday_{ time_epoch - (time_info_->tm_wday - 1) * 60 * 60 * 24 },
    time_info_monday_{ localtime(&time_epoch_monday_) } {
}

void Timetable::drawBackground() {
  // Draw OTH logo in the top left corner
  display_.drawImage(oth_logo, display_margin_, display_margin_, oth_logo_width, oth_logo_height, BLACK);

  // Draw Headline
  display_.setTextSize(text_size_headline_);
  const char* headline_1 = "Raumbelegung";
  display_.setCursor(display_.width() / 2 - int16_t(strlen(headline_1) * pixel_width_letter_ * text_size_headline_ / 2), 60);
  display_.println(headline_1);

  String headline_2_str = String(time_info_monday_->tm_mday) + "." + String(time_info_monday_->tm_mon + 1) + "." + String(time_info_monday_->tm_year + 1900) + " - " + String(time_info_monday_->tm_mday + day_number_ - 1) + "." + String(time_info_monday_->tm_mon + 1) + "." + String(time_info_monday_->tm_year + 1900);
  const char* headline_2 = headline_2_str.c_str();
  display_.setTextSize(text_size_);
  display_.setCursor(display_.width() / 2 - int16_t(strlen(headline_2) * pixel_width_letter_ * text_size_ / 2), display_.getCursorY() + 10);
  display_.println(headline_2);

  // Draw rows of table
  uint16_t y_row_position = display_.height() - display_margin_ - time_number_ * row_height_;
  for (uint8_t i = 0; i < time_number_; i++) {
    display_.drawThickLine(display_margin_, y_row_position + i * row_height_, display_margin_ + column_width_head_ + day_number_ * column_width_, y_row_position + i * row_height_, BLACK, line_thickness_);
  }

  // Draw columns of table
  uint16_t x_column_position = display_margin_ + column_width_;
  uint16_t y_column_position = display_.height() - display_margin_ - time_number_ * row_height_ - row_height_head_;
  for (uint8_t i = 0; i < day_number_; i++) {
    display_.drawThickLine(x_column_position + i * column_width_, y_column_position, x_column_position + i * column_width_, display_.height() - display_margin_, BLACK, line_thickness_);
  }
}

void Timetable::drawTimesAndDays() {
  display_.setTextSize(text_size_);

  // Draw times
  uint16_t y_times_position = display_.height() - display_margin_ - time_number_ * row_height_ + uint16_t((row_height_ / 2) - (pixel_height_letter_ / 2));
  for (uint8_t i = 0; i < time_number_; i++) {
    display_.setCursor(display_margin_ + int16_t((column_width_ / 2) - (strlen(times_[i]) * pixel_width_letter_ * text_size_ / 2)), y_times_position + i * row_height_);
    display_.print(times_[i]);
  }

  // Draw days
  uint16_t x_days_position = display_margin_ + column_width_;
  uint16_t y_days_position = display_.height() - display_margin_ - time_number_ * row_height_ - uint16_t((row_height_head_ / 2) + (pixel_height_letter_ * text_size_ / 2));
  for (uint8_t i = 0; i < day_number_; i++) {
    String day_str = String(days_[i]) + " " + String(time_info_monday_->tm_mday + i) + "." + String(time_info_monday_->tm_mon + 1) + ".";
    const char* day = day_str.c_str();
    display_.setCursor(x_days_position + i * column_width_ + int16_t((column_width_ / 2) - (strlen(day) * pixel_width_letter_ * text_size_ / 2)), y_days_position);
    display_.print(day);
  }
}

bool Timetable::compareTimes(const char* time, const char* startTime) {
  if (time[0] == '0') {
    if (time[1] == startTime[0] && time[3] == startTime[1] && time[4] == startTime[2]) {
      return true;
    } else {
      return false;
    }
  } else if (time[0] == startTime[0] && time[1] == startTime[1] && time[3] == startTime[2] && time[4] == startTime[3]) {
    return true;
  } else {
    return false;
  }
}

bool Timetable::drawData() {
  display_.setTextSize(text_size_);

  uint32_t date_monday = (1900 + time_info_monday_->tm_year) * 10000 + (time_info_monday_->tm_mon + 1) * 100 + (time_info_monday_->tm_mday);

  WebUntis webuntis_S132(display_, room_id_);
  DynamicJsonDocument json_doc = webuntis_S132.getData(time_info_monday_->tm_year + 1900, time_info_monday_->tm_mon + 1, time_info_monday_->tm_mday);

  JsonArray data_elementPeriods = json_doc["data"]["result"]["data"]["elementPeriods"][String(room_id_)].as<JsonArray>();
  JsonArray data_elements = json_doc["data"]["result"]["data"]["elements"].as<JsonArray>();

  if (data_elementPeriods.isNull() || data_elements.isNull()) {
    return false;
  } else {
    uint8_t row_index = 0;
    uint16_t x_position = display_margin_ + column_width_;
    uint16_t y_position = display_.height() - display_margin_ - time_number_ * row_height_;

    for (const char* time : times_) {
      for (JsonObject lesson : data_elementPeriods) {
        String startTime_str = lesson["startTime"].as<String>();
        const char* startTime = startTime_str.c_str();
        uint32_t date_lesson = lesson["date"].as<int>();
        uint8_t column_index = date_lesson - date_monday;

        if (compareTimes(time, startTime) && lesson_number_[row_index][column_index] == 0) {
          String docent_str = "";
          String lecture_str = "";

          for (JsonObject lesson_element : lesson["elements"].as<JsonArray>()) {
            for (JsonObject data_element : data_elements) {
              if (data_element["id"].as<int>() == lesson_element["id"].as<int>()) {
                if (data_element["type"].as<int>() == 2) {
                  docent_str = docent_str + data_element["name"].as<String>() + ", ";
                }
                if (data_element["type"].as<int>() == 3) {
                  lecture_str = data_element["alternatename"].as<String>();
                }
              }
            }
          }
          docent_str = docent_str.substring(0, docent_str.length() - 2);
          if (docent_str.length() * pixel_width_letter_ * text_size_ >= column_width_ - 2 * cell_margin_) {
            docent_str = docent_str.substring(0, uint16_t((column_width_ - 2 * cell_margin_) / (pixel_width_letter_ * text_size_)) - 2) + "..";
          }

          const char* docent = docent_str.c_str();
          const char* lecture = lecture_str.c_str();
          uint8_t line_number = 2;

          display_.setCursor(x_position + column_index * column_width_ + int16_t((column_width_ / 2) - (strlen(lecture) * pixel_width_letter_ * text_size_ / 2)), y_position + row_index * row_height_ + int16_t((row_height_ / 2) - (line_number * pixel_height_letter_ * text_size_ / 2)));
          display_.println(lecture);
          display_.setCursor(x_position + column_index * column_width_ + int16_t((column_width_ / 2) - (strlen(docent) * pixel_width_letter_ * text_size_ / 2)), display_.getCursorY());
          display_.println(docent);

          lesson_number_[row_index][column_index]++;
        }
      }
      row_index++;
    }
    return true;
  }
}
