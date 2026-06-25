#include "database.hpp"
#include <fstream>

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

  struct PairComparator {
    bool operator()(const std::pair< size_t, size_t >& a, const std::pair< size_t, size_t >& b) const
    {
      if (a.first != b.first)
        return a.first < b.first;
      return a.second < b.second;
    }
  };

  void anons(const Database& db, std::ostream& out)
  {
    size_t anonCount = 0;
    for (size_t i = 0; i < db.personsCount; ++i) {
      if (!db.persons[i].hasDescription) {
        anonCount++;
      }
    }

    if (anonCount == 0)
      return;

    size_t* anonIds = nullptr;
    try {
      anonIds = new size_t[anonCount];
      size_t idx = 0;
      for (size_t i = 0; i < db.personsCount; ++i) {
        if (!db.persons[i].hasDescription) {
          anonIds[idx++] = db.persons[i].id;
        }
      }

      for (size_t i = 0; i < anonCount; ++i) {
        for (size_t j = i + 1; j < anonCount; ++j) {
          if (anonIds[i] > anonIds[j]) {
            size_t tmp = anonIds[i];
            anonIds[i] = anonIds[j];
            anonIds[j] = tmp;
          }
        }
      }

      for (size_t i = 0; i < anonCount; ++i) {
        out << anonIds[i] << '\n';
      }

      delete[] anonIds;
    } catch (...) {
      delete[] anonIds;
      throw;
    }
  }

  bool deanon(Database& db, size_t anonId, size_t realId)
  {
    const PersonWithDesc* anon = findPerson(db, anonId);
    const PersonWithDesc* real = findPerson(db, realId);

    if (!anon || !real)
      return false;
    if (anon->hasDescription || !real->hasDescription)
      return false;

    for (size_t i = 0; i < db.meetingsCount; ++i) {
      if (db.meetings[i].id1 == anonId)
        db.meetings[i].id1 = realId;
      if (db.meetings[i].id2 == anonId)
        db.meetings[i].id2 = realId;
    }

    size_t writeIdx = 0;
    for (size_t i = 0; i < db.meetingsCount; ++i) {
      if (db.meetings[i].id1 != db.meetings[i].id2) {
        db.meetings[writeIdx++] = db.meetings[i];
      }
    }
    db.meetingsCount = writeIdx;

    writeIdx = 0;
    for (size_t i = 0; i < db.personsCount; ++i) {
      if (db.persons[i].id != anonId) {
        db.persons[writeIdx++] = db.persons[i];
      }
    }
    db.personsCount = writeIdx;

    return true;
  }

  void desc(const Database& db, size_t id, std::ostream& out)
  {
    const PersonWithDesc* p = findPerson(db, id);
    if (!p) {
      out << "<INVALID COMMAND>\n";
      return;
    }
    if (!p->hasDescription) {
      out << "<ANON>\n";
    } else {
      out << p->info << '\n';
    }
  }

  bool redesc(Database& db, size_t id, const std::string& desc)
  {
    PersonWithDesc* p = const_cast< PersonWithDesc* >(findPerson(db, id));
    if (!p)
      return false;
    p->info = desc;
    p->hasDescription = true;
    return true;
  }

  void meets(const Database& db, size_t id, std::ostream& out)
  {
    if (!findPerson(db, id)) {
      out << "<INVALID COMMAND>\n";
      return;
    }

    size_t meetCount = 0;
    for (size_t i = 0; i < db.meetingsCount; ++i) {
      if (db.meetings[i].id1 == id || db.meetings[i].id2 == id) {
        meetCount++;
      }
    }

    if (meetCount == 0)
      return;

    std::pair< size_t, size_t >* result = nullptr;
    try {
      result = new std::pair< size_t, size_t >[meetCount];
      size_t idx = 0;
      for (size_t i = 0; i < db.meetingsCount; ++i) {
        if (db.meetings[i].id1 == id) {
          result[idx++] = std::make_pair(db.meetings[i].id2, db.meetings[i].duration);
        } else if (db.meetings[i].id2 == id) {
          result[idx++] = std::make_pair(db.meetings[i].id1, db.meetings[i].duration);
        }
      }

      PairComparator comp;
      for (size_t i = 0; i < meetCount; ++i) {
        for (size_t j = i + 1; j < meetCount; ++j) {
          if (comp(result[j], result[i])) {
            std::pair< size_t, size_t > tmp = result[i];
            result[i] = result[j];
            result[j] = tmp;
          }
        }
      }

      for (size_t i = 0; i < meetCount; ++i) {
        out << result[i].first << ' ' << result[i].second << '\n';
      }

      delete[] result;
    } catch (...) {
      delete[] result;
      throw;
    }
  }

  void commons(const Database& db, size_t id1, size_t id2, std::ostream& out)
  {
    if (!findPerson(db, id1) || !findPerson(db, id2)) {
      out << "<INVALID COMMAND>\n";
      return;
    }

    size_t* meets1 = nullptr;
    size_t meets1Count = 0;
    size_t meets1Cap = 0;

    size_t* meets2 = nullptr;
    size_t meets2Count = 0;
    size_t meets2Cap = 0;

    try {
      meets1Cap = 8;
      meets1 = new size_t[meets1Cap];
      meets2Cap = 8;
      meets2 = new size_t[meets2Cap];

      for (size_t i = 0; i < db.meetingsCount; ++i) {
        size_t partner = 0;
        bool found = false;

        if (db.meetings[i].id1 == id1) {
          partner = db.meetings[i].id2;
          found = true;
        } else if (db.meetings[i].id2 == id1) {
          partner = db.meetings[i].id1;
          found = true;
        }

        if (found) {
          bool exists = false;
          for (size_t j = 0; j < meets1Count; ++j) {
            if (meets1[j] == partner) {
              exists = true;
              break;
            }
          }
          if (!exists) {
            if (meets1Count >= meets1Cap) {
              size_t* tmp = new size_t[meets1Cap * 2];
              for (size_t j = 0; j < meets1Count; ++j)
                tmp[j] = meets1[j];
              delete[] meets1;
              meets1 = tmp;
              meets1Cap *= 2;
            }
            meets1[meets1Count++] = partner;
          }
        }

        found = false;
        if (db.meetings[i].id1 == id2) {
          partner = db.meetings[i].id2;
          found = true;
        } else if (db.meetings[i].id2 == id2) {
          partner = db.meetings[i].id1;
          found = true;
        }

        if (found) {
          bool exists = false;
          for (size_t j = 0; j < meets2Count; ++j) {
            if (meets2[j] == partner) {
              exists = true;
              break;
            }
          }
          if (!exists) {
            if (meets2Count >= meets2Cap) {
              size_t* tmp = new size_t[meets2Cap * 2];
              for (size_t j = 0; j < meets2Count; ++j)
                tmp[j] = meets2[j];
              delete[] meets2;
              meets2 = tmp;
              meets2Cap *= 2;
            }
            meets2[meets2Count++] = partner;
          }
        }
      }

      size_t* common = nullptr;
      size_t commonCount = 0;
      size_t commonCap = 8;
      common = new size_t[commonCap];

      for (size_t i = 0; i < meets1Count; ++i) {
        for (size_t j = 0; j < meets2Count; ++j) {
          if (meets1[i] == meets2[j]) {
            if (commonCount >= commonCap) {
              size_t* tmp = new size_t[commonCap * 2];
              for (size_t k = 0; k < commonCount; ++k)
                tmp[k] = common[k];
              delete[] common;
              common = tmp;
              commonCap *= 2;
            }
            common[commonCount++] = meets1[i];
            break;
          }
        }
      }

      for (size_t i = 0; i < commonCount; ++i) {
        for (size_t j = i + 1; j < commonCount; ++j) {
          if (common[i] > common[j]) {
            size_t tmp = common[i];
            common[i] = common[j];
            common[j] = tmp;
          }
        }
      }

      for (size_t i = 0; i < commonCount; ++i) {
        out << common[i] << '\n';
      }

      delete[] common;
      delete[] meets1;
      delete[] meets2;
    } catch (...) {
      delete[] meets1;
      delete[] meets2;
      throw;
    }
  }

  void less(const Database& db, size_t time, size_t id, std::ostream& out)
  {
    if (!findPerson(db, id)) {
      out << "<INVALID COMMAND>\n";
      return;
    }

    size_t meetCount = 0;
    for (size_t i = 0; i < db.meetingsCount; ++i) {
      if (db.meetings[i].duration < time) {
        if (db.meetings[i].id1 == id || db.meetings[i].id2 == id) {
          meetCount++;
        }
      }
    }

    if (meetCount == 0)
      return;

    std::pair< size_t, size_t >* result = nullptr;
    try {
      result = new std::pair< size_t, size_t >[meetCount];
      size_t idx = 0;
      for (size_t i = 0; i < db.meetingsCount; ++i) {
        if (db.meetings[i].duration < time) {
          if (db.meetings[i].id1 == id) {
            result[idx++] = std::make_pair(db.meetings[i].id2, db.meetings[i].duration);
          } else if (db.meetings[i].id2 == id) {
            result[idx++] = std::make_pair(db.meetings[i].id1, db.meetings[i].duration);
          }
        }
      }

      PairComparator comp;
      for (size_t i = 0; i < meetCount; ++i) {
        for (size_t j = i + 1; j < meetCount; ++j) {
          if (comp(result[j], result[i])) {
            std::pair< size_t, size_t > tmp = result[i];
            result[i] = result[j];
            result[j] = tmp;
          }
        }
      }

      for (size_t i = 0; i < meetCount; ++i) {
        out << result[i].first << ' ' << result[i].second << '\n';
      }

      delete[] result;
    } catch (...) {
      delete[] result;
      throw;
    }
  }

  void greater(const Database& db, size_t time, size_t id, std::ostream& out)
  {
    if (!findPerson(db, id)) {
      out << "<INVALID COMMAND>\n";
      return;
    }

    size_t meetCount = 0;
    for (size_t i = 0; i < db.meetingsCount; ++i) {
      if (db.meetings[i].duration > time) {
        if (db.meetings[i].id1 == id || db.meetings[i].id2 == id) {
          meetCount++;
        }
      }
    }

    if (meetCount == 0)
      return;

    std::pair< size_t, size_t >* result = nullptr;
    try {
      result = new std::pair< size_t, size_t >[meetCount];
      size_t idx = 0;
      for (size_t i = 0; i < db.meetingsCount; ++i) {
        if (db.meetings[i].duration > time) {
          if (db.meetings[i].id1 == id) {
            result[idx++] = std::make_pair(db.meetings[i].id2, db.meetings[i].duration);
          } else if (db.meetings[i].id2 == id) {
            result[idx++] = std::make_pair(db.meetings[i].id1, db.meetings[i].duration);
          }
        }
      }

      PairComparator comp;
      for (size_t i = 0; i < meetCount; ++i) {
        for (size_t j = i + 1; j < meetCount; ++j) {
          if (comp(result[j], result[i])) {
            std::pair< size_t, size_t > tmp = result[i];
            result[i] = result[j];
            result[j] = tmp;
          }
        }
      }

      for (size_t i = 0; i < meetCount; ++i) {
        out << result[i].first << ' ' << result[i].second << '\n';
      }

      delete[] result;
    } catch (...) {
      delete[] result;
      throw;
    }
  }

  void outPersons(const Database& db, const std::string& filename)
  {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
      std::cerr << "Error: cannot open output file\n";
      return;
    }

    for (size_t i = 0; i < db.personsCount; ++i) {
      if (db.persons[i].hasDescription) {
        outFile << db.persons[i].id << ' ' << db.persons[i].info << '\n';
      }
    }
  }
}
