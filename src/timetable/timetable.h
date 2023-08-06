/**********************************************************
* Include Guard
**********************************************************/
#ifndef TIMETABLE_H_
#define TIMETABLE_H_

/**********************************************************
* Private Includes
**********************************************************/
#include <Inkplate.h>
#include <time.h>
#include <stdint.h>

/*********************************************************/

class Timetable {
/**********************************************************
* Public Function Prototypes
**********************************************************/
public:
  Timetable(Inkplate& display, uint16_t room_id, time_t time_epoch);
  void drawBackground();
  void drawTimesAndDays();
  bool drawData();
  bool compareTimes(const char* time, const char* startTime);
  struct tm addDaysToTM(struct tm* time_info, uint8_t days_to_add);

/**********************************************************
* Private Member Variables
**********************************************************/
private:
  Inkplate& display_;                                     // reference of the Inkplate object
  uint16_t room_id_;                                      // WebUntis ID of the room for the timetable

  uint8_t text_size_;                                     // text size of the standard text
  uint8_t text_size_headline_;                            // text size of the headline text
  uint8_t pixel_width_letter_;                            // pixel width of a letter with the text size 1
  uint8_t pixel_height_letter_;                           // pixel height of a letter with the text size 1

  uint8_t display_margin_;                                // spacing between the display border and the display content
  uint8_t cell_margin_;                                   // spacing between the table cell border and the table cell content
  uint8_t line_thickness_;                                // thickness of the table lines
  uint8_t row_height_head_;                               // height of the head row of the table
  uint8_t row_height_;                                    // height of normal a row of the table
  uint8_t column_width_head_;                             // width of the head column of the table
  uint8_t column_width_;                                  // width of normal a column of the table

  static const uint8_t time_number_ = 7;                  // number of time ranges specified in the variable times_
  static const uint8_t day_number_ = 6;                   // number of days specified in the variable days_

  const char* times_[time_number_];                       // time ranges shown in the timetable
  const char* days_[day_number_];                         // days shown in the timetable
  uint8_t lesson_number_[time_number_][day_number_];      // number of lessons in one table cell

  time_t time_epoch_;                                     // time for the timetable data as unix epoch
  struct tm* time_info_;                                  // time for the timetable data as struct tm*
  time_t time_epoch_monday_;                              // time of the monday of the week in the time for the timetable data as unix epoch
  struct tm* time_info_monday_;                           // time of the monday of the week in the time for the timetable data as struct tm*
};

#endif
