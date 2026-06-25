#ifndef COMMON_DATABASE_HPP
#define COMMON_DATABASE_HPP

#include "types.hpp"
#include <iostream>

namespace hvostov {
  struct Database {
    PersonWithDesc* persons;
    size_t personsCount;
    size_t personsCapacity;
    Meeting* meetings;
    size_t meetingsCount;
    size_t meetingsCapacity;
  };

  void initDatabase(Database& db);
  void freeDatabase(Database& db);
  void addPerson(Database& db, const Person& p);
  void addPersonWithDesc(Database& db, const PersonWithDesc& p);
  void addMeeting(Database& db, const Meeting& m);
  const PersonWithDesc* findPerson(const Database& db, size_t id);
  bool hasDescription(const Database& db, size_t id);
  void anons(const Database& db, std::ostream& out);
  bool deanon(Database& db, size_t anonId, size_t realId);
  void desc(const Database& db, size_t id, std::ostream& out);
  bool redesc(Database& db, size_t id, const std::string& desc);
  void meets(const Database& db, size_t id, std::ostream& out);
  void commons(const Database& db, size_t id1, size_t id2, std::ostream& out);
  void less(const Database& db, size_t time, size_t id, std::ostream& out);
  void greater(const Database& db, size_t time, size_t id, std::ostream& out);
  void outPersons(const Database& db, const std::string& filename);
}

#endif
