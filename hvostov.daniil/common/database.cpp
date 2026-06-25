#include "database.hpp"

namespace hvostov {
  void initDatabase(Database& db)
  {
    db.persons = nullptr;
    db.personsCount = 0;
    db.personsCapacity = 0;
    db.meetings = nullptr;
    db.meetingsCount = 0;
    db.meetingsCapacity = 0;
  }

  void freeDatabase(Database& db)
  {
    delete[] db.persons;
    delete[] db.meetings;
    initDatabase(db);
  }

  void addPerson(Database& db, const Person& p)
  {
    PersonWithDesc pwd;
    pwd.id = p.id;
    pwd.info = p.info;
    pwd.hasDescription = true;
    addPersonWithDesc(db, pwd);
  }

  void addPersonWithDesc(Database& db, const PersonWithDesc& p)
  {
    for (size_t i = 0; i < db.personsCount; ++i) {
      if (db.persons[i].id == p.id) {
        return;
      }
    }

    if (db.personsCount >= db.personsCapacity) {
      size_t newCapacity = db.personsCapacity == 0 ? 8 : db.personsCapacity * 2;
      PersonWithDesc* newPersons = nullptr;
      try {
        newPersons = new PersonWithDesc[newCapacity];
        for (size_t i = 0; i < db.personsCount; ++i) {
          newPersons[i] = db.persons[i];
        }
        delete[] db.persons;
        db.persons = newPersons;
        db.personsCapacity = newCapacity;
      } catch (...) {
        delete[] newPersons;
        throw;
      }
    }

    db.persons[db.personsCount++] = p;
  }

  void addMeeting(Database& db, const Meeting& m)
  {
    if (db.meetingsCount >= db.meetingsCapacity) {
      size_t newCapacity = db.meetingsCapacity == 0 ? 8 : db.meetingsCapacity * 2;
      Meeting* newMeetings = nullptr;
      try {
        newMeetings = new Meeting[newCapacity];
        for (size_t i = 0; i < db.meetingsCount; ++i) {
          newMeetings[i] = db.meetings[i];
        }
        delete[] db.meetings;
        db.meetings = newMeetings;
        db.meetingsCapacity = newCapacity;
      } catch (...) {
        delete[] newMeetings;
        throw;
      }
    }

    db.meetings[db.meetingsCount++] = m;

    PersonWithDesc p1 = {m.id1, "", false};
    PersonWithDesc p2 = {m.id2, "", false};
    addPersonWithDesc(db, p1);
    addPersonWithDesc(db, p2);
  }

  const PersonWithDesc* findPerson(const Database& db, size_t id)
  {
    for (size_t i = 0; i < db.personsCount; ++i) {
      if (db.persons[i].id == id) {
        return &db.persons[i];
      }
    }
    return nullptr;
  }

  bool hasDescription(const Database& db, size_t id)
  {
    const PersonWithDesc* p = findPerson(db, id);
    return p != nullptr && p->hasDescription;
  }

  // ... все остальные функции из U2/database.cpp ...
  // anons, deanon, desc, redesc, meets, commons, less, greater, outPersons
}
