#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "types.hpp"
#include <iostream>

namespace hvostov {
  // Инициализация и очистка
  void initDatabase(Database& db);
  void freeDatabase(Database& db);

  // Добавление данных
  void addPerson(Database& db, const Person& p);
  void addMeeting(Database& db, const Meeting& m);

  // Поиск
  const Person* findPerson(const Database& db, size_t id);
  bool hasDescription(const Database& db, size_t id);

  // Команды
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
