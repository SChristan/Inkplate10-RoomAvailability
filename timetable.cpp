/**********************************************************
* Private Includes
**********************************************************/
#include "timetable.h"
#include "oth_logo.h"
#include "webuntis.h"
#include <string.h>

/**********************************************************
* Class Constructors
**********************************************************/
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

/*********************************************************/

/** @brief Drawing the background for the timetable. Including the OTH logo, the headline with time period and the rows and columns of the table.
 */
void Timetable::drawBackground() {
  // Draw OTH logo in the top left corner
  display_.drawImage(oth_logo, display_margin_, display_margin_, oth_logo_width, oth_logo_height, BLACK);

  // Draw Headline
  display_.setTextSize(text_size_headline_);
  const char* headline_1 = "Raumbelegung";
  // Sets the cursor on the display for the headline, the horizontal position is calculated to center the text
  display_.setCursor(display_.width() / 2 - int16_t(strlen(headline_1) * pixel_width_letter_ * text_size_headline_ / 2), 60);
  display_.println(headline_1);

  // Draw the time range of the timetable underneath the headline
  struct tm time = addDaysToTM(time_info_monday_, day_number_ - 1);
  String headline_2_str = String(time_info_monday_->tm_mday) + "." + String(time_info_monday_->tm_mon + 1) + "." + String(time_info_monday_->tm_year + 1900) + " - " + String(time.tm_mday) + "." + String(time.tm_mon + 1) + "." + String(time.tm_year + 1900);
  const char* headline_2 = headline_2_str.c_str();
  display_.setTextSize(text_size_);
  // Sets the cursor on the display for the time range, the horizontal position is calculated to center the text
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

/*********************************************************/

/** @brief Drawing the times and days for the timetable in the head row and head column of the table, respectively.
 */
void Timetable::drawTimesAndDays() {
  display_.setTextSize(text_size_);

  // Draw times
  // Calculation of the position of the first time, centering in the vertical
  uint16_t y_times_position = display_.height() - display_margin_ - time_number_ * row_height_ + uint16_t((row_height_ / 2) - (pixel_height_letter_ / 2));
  // Loop with an iteration for each time
  for (uint8_t i = 0; i < time_number_; i++) {
  // Sets the cursor on the display for the times, the horizontal position is calculated to center the text
    display_.setCursor(display_margin_ + int16_t((column_width_ / 2) - (strlen(times_[i]) * pixel_width_letter_ * text_size_ / 2)), y_times_position + i * row_height_);
    display_.print(times_[i]);
  }

  // Draw days
  // Offset of the first normal column without the head column
  uint16_t x_days_position = display_margin_ + column_width_;
  // Calculation of the vertical position of the days, centering in the vertical
  uint16_t y_days_position = display_.height() - display_margin_ - time_number_ * row_height_ - uint16_t((row_height_head_ / 2) + (pixel_height_letter_ * text_size_ / 2));
  // Loop with an iteration for each day
  for (uint8_t i = 0; i < day_number_; i++) {
    // Create the time for each day
    struct tm time = addDaysToTM(time_info_monday_, i);
    // Create the string with the day and time
    String day_str = String(days_[i]) + " " + String(time.tm_mday) + "." + String(time.tm_mon + 1) + ".";
    const char* day = day_str.c_str();
    // Sets the cursor on the display for the times, the horizontal position is calculated to center the text
    display_.setCursor(x_days_position + i * column_width_ + int16_t((column_width_ / 2) - (strlen(day) * pixel_width_letter_ * text_size_ / 2)), y_days_position);
    display_.print(day);
  }
}

/*********************************************************/

/** @brief Loading the data from WebUntis and draw them on the display. Includes lesson and docent abbreviation.
 *
 *  @return Boolean value whether the data was loaded successfully.
 */
bool Timetable::drawData() {
  display_.setTextSize(text_size_);

  // Create the date as a number so that it looks like a string
  uint32_t date_monday = (1900 + time_info_monday_->tm_year) * 10000 + (time_info_monday_->tm_mon + 1) * 100 + (time_info_monday_->tm_mday);

  // Loading the data from WebUntis
  WebUntis webuntis_S132(display_, room_id_);
  DynamicJsonDocument json_doc = webuntis_S132.getData(time_info_monday_->tm_year + 1900, time_info_monday_->tm_mon + 1, time_info_monday_->tm_mday);

  // Filter the data to get the elementPeriods and elements array, respectively.
  JsonArray data_elementPeriods = json_doc["data"]["result"]["data"]["elementPeriods"][String(room_id_)].as<JsonArray>();
  JsonArray data_elements = json_doc["data"]["result"]["data"]["elements"].as<JsonArray>();

  // Check if the data was loaded successfully
  if (data_elementPeriods.isNull() || data_elements.isNull()) {
    return false;
  } else {
    uint8_t row_index = 0;                                                                        // Indicates the current row
    uint16_t x_position = display_margin_ + column_width_;                                        // Offset of the first normal column without the head column
    uint16_t y_position = display_.height() - display_margin_ - time_number_ * row_height_;       // Offset of the first normal row without the head row

    // Iterating through the whole table
    // Iterating through every row
    for (const char* time : times_) {
      // Iterating through every lesson found in WebUntis data and paste it in the correct column
      for (JsonObject lesson : data_elementPeriods) {
        String startTime_str = lesson["startTime"].as<String>();  // Start time of the lesson
        const char* startTime = startTime_str.c_str();
        uint32_t date_lesson = lesson["date"].as<int>();          // Date of the lesson
        uint8_t column_index = date_lesson - date_monday;         // Column index in which the lesson data will be inserted

        // Check if the time of the row and the time of the lesson match and check if the lesson has already been worked through.
        if (compareTimes(time, startTime) && lesson_number_[row_index][column_index] == 0) {
          // Create empty string variables for concatenation
          String docent_str = "";
          String lecture_str = "";

          // Iterating through every elements containing the related data of the lesson as an ID
          for (JsonObject lesson_element : lesson["elements"].as<JsonArray>()) {
            // Iterating through every elements of the WebUntis data, which contain the info as string about an element ID
            for (JsonObject data_element : data_elements) {
              // Check if the IDs match, if so, retrieve the information as a string.
              if (data_element["id"].as<int>() == lesson_element["id"].as<int>()) {
                // The ID 2 contains information about the docent
                if (data_element["type"].as<int>() == 2) {
                  docent_str = docent_str + data_element["name"].as<String>() + ", ";
                }
                // The ID 3 contains information about the lesson name
                if (data_element["type"].as<int>() == 3) {
                  lecture_str = data_element["alternatename"].as<String>();
                }
              }
            }
          }
          // Trim the colon and the space at the end of the string
          docent_str = docent_str.substring(0, docent_str.length() - 2);
          // Trim the length of the string to the maximum width of a table cell, so it doesn't overflow
          if (docent_str.length() * pixel_width_letter_ * text_size_ >= column_width_ - 2 * cell_margin_) {
            docent_str = docent_str.substring(0, uint16_t((column_width_ - 2 * cell_margin_) / (pixel_width_letter_ * text_size_)) - 2) + "..";
          }

          const char* docent = docent_str.c_str();
          const char* lecture = lecture_str.c_str();
          uint8_t line_number = 2;

          // Set the position of the cursor on the display, centering the text horizontally and vertically
          display_.setCursor(x_position + column_index * column_width_ + int16_t((column_width_ / 2) - (strlen(lecture) * pixel_width_letter_ * text_size_ / 2)), y_position + row_index * row_height_ + int16_t((row_height_ / 2) - (line_number * pixel_height_letter_ * text_size_ / 2)));
          display_.println(lecture);
          // Set the position of the cursor on the display, centering the text horizontally and vertically
          display_.setCursor(x_position + column_index * column_width_ + int16_t((column_width_ / 2) - (strlen(docent) * pixel_width_letter_ * text_size_ / 2)), display_.getCursorY());
          display_.println(docent);

          // Increment the lesson number to indicate that the lesson has already been worked through
          lesson_number_[row_index][column_index]++;
        }
      }
      // Increment the row index to process the next row
      row_index++;
    }
    return true;
  }
}

/*********************************************************/

/** @brief Comparison of two times as char pointer. Specifically the time from WebUntis and the time of the timetable.
 *
 *  @return Boolean value whether the times match.
 */
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

/*********************************************************/

/** @brief Adding days to a given time as struct tm.
 *
 *  @return Struct tm with the additional number of days.
 */
struct tm Timetable::addDaysToTM(struct tm* time_info, uint8_t days_to_add) {
  time_t epochs = mktime(time_info);
  struct tm time = *localtime(&epochs);
  time.tm_mday += days_to_add;
  mktime(&time);
  return time;
}
