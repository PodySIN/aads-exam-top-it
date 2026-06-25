#ifndef U3_TYPES_HPP
#define U3_TYPES_HPP

#include <types.hpp>
#include <iostream>

namespace hvostov {
  struct Date {
    size_t day;
    size_t month;
    size_t year;
  };

  struct MeetingWithDate {
    size_t id1;
    size_t id2;
    size_t duration;
    Date date;
  };

  struct DateRange {
    Date* dates;
    size_t count;
    size_t capacity;
  };

  struct RangeStack {
    DateRange* ranges;
    size_t count;
    size_t capacity;
  };

  std::istream& operator>>(std::istream& in, Date& dest);
  std::istream& operator>>(std::istream& in, MeetingWithDate& dest);
  std::ostream& operator<<(std::ostream& out, const Date& dest);

  bool isDateLess(const Date& a, const Date& b);
  bool isDateEqual(const Date& a, const Date& b);
  bool isDateInRange(const Date& date, const DateRange& range);
}

#endif
