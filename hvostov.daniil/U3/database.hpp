#ifndef U3_DATABASE_HPP
#define U3_DATABASE_HPP

#include <database.hpp>
#include "types.hpp"
#include <iostream>

namespace hvostov {
  struct DatabaseU3 {
    Database base;
    DateRange allDates;
    DateRange currentRange;
    RangeStack rangeHistory;
    MeetingWithDate* meetingsWithDate;
    size_t meetingsWithDateCount;
    size_t meetingsWithDateCapacity;
  };

  void initDatabaseU3(DatabaseU3& db);
  void freeDatabaseU3(DatabaseU3& db);
  void addMeetingU3(DatabaseU3& db, const MeetingWithDate& m);
  void addDate(DatabaseU3& db, const Date& date);
  void initDateRange(DatabaseU3& db);
  void range(const DatabaseU3& db, std::ostream& out);
  bool after(DatabaseU3& db, const Date& date);
  bool before(DatabaseU3& db, const Date& date);
  bool popRange(DatabaseU3& db);
}

#endif
