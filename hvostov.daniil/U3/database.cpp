#include "database.hpp"

namespace hvostov {
  void initDatabaseU3(DatabaseU3& db)
  {
    initDatabase(db.base);
    db.allDates.dates = nullptr;
    db.allDates.count = 0;
    db.allDates.capacity = 0;
    db.currentRange.dates = nullptr;
    db.currentRange.count = 0;
    db.currentRange.capacity = 0;
    db.rangeHistory.ranges = nullptr;
    db.rangeHistory.count = 0;
    db.rangeHistory.capacity = 0;
    db.meetingsWithDate = nullptr;
    db.meetingsWithDateCount = 0;
    db.meetingsWithDateCapacity = 0;
  }

  void freeDatabaseU3(DatabaseU3& db)
  {
    freeDatabase(db.base);
    delete[] db.allDates.dates;
    delete[] db.currentRange.dates;
    if (db.rangeHistory.ranges) {
      for (size_t i = 0; i < db.rangeHistory.count; ++i) {
        delete[] db.rangeHistory.ranges[i].dates;
      }
      delete[] db.rangeHistory.ranges;
    }
    delete[] db.meetingsWithDate;
    initDatabaseU3(db);
  }

  void addDate(DatabaseU3& db, const Date& date)
  {
    if (db.allDates.count >= db.allDates.capacity) {
      size_t newCap = db.allDates.capacity == 0 ? 8 : db.allDates.capacity * 2;
      Date* tmp = new Date[newCap];
      for (size_t i = 0; i < db.allDates.count; ++i) {
        tmp[i] = db.allDates.dates[i];
      }
      delete[] db.allDates.dates;
      db.allDates.dates = tmp;
      db.allDates.capacity = newCap;
    }

    for (size_t i = 0; i < db.allDates.count; ++i) {
      if (isDateEqual(db.allDates.dates[i], date)) {
        return;
      }
    }

    db.allDates.dates[db.allDates.count++] = date;

    for (size_t i = 0; i < db.allDates.count; ++i) {
      for (size_t j = i + 1; j < db.allDates.count; ++j) {
        if (isDateLess(db.allDates.dates[j], db.allDates.dates[i])) {
          Date tmp = db.allDates.dates[i];
          db.allDates.dates[i] = db.allDates.dates[j];
          db.allDates.dates[j] = tmp;
        }
      }
    }
  }

  void addMeetingU3(DatabaseU3& db, const MeetingWithDate& m)
  {
    if (db.meetingsWithDateCount >= db.meetingsWithDateCapacity) {
      size_t newCapacity = db.meetingsWithDateCapacity == 0 ? 8 : db.meetingsWithDateCapacity * 2;
      MeetingWithDate* newMeetings = nullptr;
      try {
        newMeetings = new MeetingWithDate[newCapacity];
        for (size_t i = 0; i < db.meetingsWithDateCount; ++i) {
          newMeetings[i] = db.meetingsWithDate[i];
        }
        delete[] db.meetingsWithDate;
        db.meetingsWithDate = newMeetings;
        db.meetingsWithDateCapacity = newCapacity;
      } catch (...) {
        delete[] newMeetings;
        throw;
      }
    }

    db.meetingsWithDate[db.meetingsWithDateCount++] = m;
    addDate(db, m.date);

    Meeting baseMeeting = {m.id1, m.id2, m.duration};
    addMeeting(db.base, baseMeeting);
  }

  void initDateRange(DatabaseU3& db)
  {
    delete[] db.currentRange.dates;
    db.currentRange.count = db.allDates.count;
    db.currentRange.capacity = db.allDates.capacity;

    if (db.allDates.count > 0) {
      db.currentRange.dates = new Date[db.allDates.capacity];
      for (size_t i = 0; i < db.allDates.count; ++i) {
        db.currentRange.dates[i] = db.allDates.dates[i];
      }
    } else {
      db.currentRange.dates = nullptr;
    }
  }

  void range(const DatabaseU3& db, std::ostream& out)
  {
    if (db.currentRange.count == 0) {
      out << "<EMPTY>\n";
      return;
    }

    out << db.currentRange.dates[0] << " : " << db.currentRange.dates[db.currentRange.count - 1] << '\n';
  }

  bool after(DatabaseU3& db, const Date& date)
  {
    if (db.currentRange.count == 0) {
      return false;
    }

    if (db.rangeHistory.count >= db.rangeHistory.capacity) {
      size_t newCap = db.rangeHistory.capacity == 0 ? 4 : db.rangeHistory.capacity * 2;
      DateRange* tmp = new DateRange[newCap];
      for (size_t i = 0; i < db.rangeHistory.count; ++i) {
        tmp[i] = db.rangeHistory.ranges[i];
      }
      delete[] db.rangeHistory.ranges;
      db.rangeHistory.ranges = tmp;
      db.rangeHistory.capacity = newCap;
    }

    DateRange saved;
    saved.count = db.currentRange.count;
    saved.capacity = db.currentRange.capacity;
    saved.dates = new Date[saved.capacity];
    for (size_t i = 0; i < saved.count; ++i) {
      saved.dates[i] = db.currentRange.dates[i];
    }
    db.rangeHistory.ranges[db.rangeHistory.count++] = saved;

    size_t writeIdx = 0;
    for (size_t i = 0; i < db.currentRange.count; ++i) {
      if (!isDateLess(db.currentRange.dates[i], date)) {
        db.currentRange.dates[writeIdx++] = db.currentRange.dates[i];
      }
    }
    db.currentRange.count = writeIdx;

    return true;
  }

  bool before(DatabaseU3& db, const Date& date)
  {
    if (db.currentRange.count == 0) {
      return false;
    }

    if (db.rangeHistory.count >= db.rangeHistory.capacity) {
      size_t newCap = db.rangeHistory.capacity == 0 ? 4 : db.rangeHistory.capacity * 2;
      DateRange* tmp = new DateRange[newCap];
      for (size_t i = 0; i < db.rangeHistory.count; ++i) {
        tmp[i] = db.rangeHistory.ranges[i];
      }
      delete[] db.rangeHistory.ranges;
      db.rangeHistory.ranges = tmp;
      db.rangeHistory.capacity = newCap;
    }

    DateRange saved;
    saved.count = db.currentRange.count;
    saved.capacity = db.currentRange.capacity;
    saved.dates = new Date[saved.capacity];
    for (size_t i = 0; i < saved.count; ++i) {
      saved.dates[i] = db.currentRange.dates[i];
    }
    db.rangeHistory.ranges[db.rangeHistory.count++] = saved;

    size_t writeIdx = 0;
    for (size_t i = 0; i < db.currentRange.count; ++i) {
      if (!isDateLess(date, db.currentRange.dates[i])) {
        db.currentRange.dates[writeIdx++] = db.currentRange.dates[i];
      }
    }
    db.currentRange.count = writeIdx;

    return true;
  }

  bool popRange(DatabaseU3& db)
  {
    if (db.rangeHistory.count == 0) {
      return false;
    }

    delete[] db.currentRange.dates;
    DateRange prev = db.rangeHistory.ranges[--db.rangeHistory.count];
    db.currentRange.dates = prev.dates;
    db.currentRange.count = prev.count;
    db.currentRange.capacity = prev.capacity;

    return true;
  }
}
