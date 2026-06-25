#include "types.hpp"

std::istream& hvostov::operator>>(std::istream& in, Date& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry) {
    return in;
  }

  Date input;

  if (!(in >> input.day >> input.month >> input.year)) {
    return in;
  }

  dest = input;
  return in;
}

std::ostream& hvostov::operator<<(std::ostream& out, const Date& dest)
{
  out << dest.day << ' ' << dest.month << ' ' << dest.year;
  return out;
}

std::istream& hvostov::operator>>(std::istream& in, MeetingWithDate& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry) {
    return in;
  }

  MeetingWithDate input;

  if (!(in >> input.date >> input.id1 >> input.id2 >> input.duration)) {
    return in;
  }

  if (input.id1 == input.id2) {
    in.setstate(std::ios::failbit);
    return in;
  }

  dest = input;
  return in;
}

bool hvostov::isDateLess(const Date& a, const Date& b)
{
  if (a.year != b.year)
    return a.year < b.year;
  if (a.month != b.month)
    return a.month < b.month;
  return a.day < b.day;
}

bool hvostov::isDateEqual(const Date& a, const Date& b)
{
  return a.year == b.year && a.month == b.month && a.day == b.day;
}

bool hvostov::isDateInRange(const Date& date, const DateRange& range)
{
  if (range.count == 0)
    return false;
  return !isDateLess(date, range.dates[0]) && !isDateLess(range.dates[range.count - 1], date);
}
