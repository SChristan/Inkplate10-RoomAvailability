#ifndef TIMETABLE_H_
#define TIMETABLE_H_

#include <Inkplate.h>
#include <time.h>
#include <stdint.h>

class Timetable {
public:
  Timetable(Inkplate& display, uint16_t room_id, time_t time_epoch);
  void drawBackground();
  void drawTimesAndDays();
  bool drawData();
  bool compareTimes(const char* time, const char* startTime);
  struct tm addDaysToTM(struct tm* time_info, uint8_t days_to_add);

private:
  Inkplate& display_;
  uint16_t room_id_;

  uint8_t text_size_;
  uint8_t text_size_headline_;
  uint8_t pixel_width_letter_;
  uint8_t pixel_height_letter_;

  uint8_t display_margin_;
  uint8_t cell_margin_;
  uint8_t line_thickness_;
  uint8_t row_height_head_;
  uint8_t row_height_;
  uint8_t column_width_head_;
  uint8_t column_width_;

  static const uint8_t time_number_ = 7;
  static const uint8_t day_number_ = 6;

  const char* times_[time_number_];
  const char* days_[day_number_];
  uint8_t lesson_number_[time_number_][day_number_];

  time_t time_epoch_;
  struct tm* time_info_;
  time_t time_epoch_monday_;
  struct tm* time_info_monday_;
};

#endif
