#include "database.hpp"
#include <utils.hpp>
#include <fstream>

void hvostov::initDatabase(Database& db)
{
  db.persons = nullptr;
  db.personsCount = 0;
  db.personsCapacity = 0;
  db.meetings = nullptr;
  db.meetingsCount = 0;
  db.meetingsCapacity = 0;
}

void hvostov::freeDatabase(Database& db)
{
  delete[] db.persons;
  delete[] db.meetings;
  initDatabase(db);
}

void hvostov::addPerson(Database& db, const Person& p)
{
  PersonWithDesc pwd;
  pwd.id = p.id;
  pwd.info = p.info;
  pwd.hasDescription = true;
  addPersonWithDesc(db, pwd);
}

void hvostov::addPersonWithDesc(Database& db, const PersonWithDesc& p)
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

void hvostov::addMeeting(Database& db, const Meeting& m)
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

const hvostov::PersonWithDesc* hvostov::findPerson(const Database& db, size_t id)
{
  for (size_t i = 0; i < db.personsCount; ++i) {
    if (db.persons[i].id == id) {
      return &db.persons[i];
    }
  }
  return nullptr;
}

bool hvostov::hasDescription(const Database& db, size_t id)
{
  const PersonWithDesc* p = findPerson(db, id);
  return p != nullptr && p->hasDescription;
}

void hvostov::anons(const Database& db, std::ostream& out)
{
  size_t anonCount = 0;
  for (size_t i = 0; i < db.personsCount; ++i) {
    if (!db.persons[i].hasDescription) {
      anonCount++;
    }
  }

  if (anonCount == 0) {
    out << '\n';
    return;
  }

  size_t* anonIds = nullptr;
  try {
    anonIds = new size_t[anonCount];
    size_t idx = 0;
    for (size_t i = 0; i < db.personsCount; ++i) {
      if (!db.persons[i].hasDescription) {
        anonIds[idx++] = db.persons[i].id;
      }
    }

    sortArray(anonIds, anonCount);
    printArray(anonIds, anonCount, out);

    delete[] anonIds;
  } catch (...) {
    delete[] anonIds;
    throw;
  }
}

bool hvostov::deanon(Database& db, size_t anonId, size_t realId)
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

void hvostov::desc(const Database& db, size_t id, std::ostream& out)
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

bool hvostov::redesc(Database& db, size_t id, const std::string& desc)
{
  PersonWithDesc* p = const_cast< PersonWithDesc* >(findPerson(db, id));
  if (!p)
    return false;
  p->info = desc;
  p->hasDescription = true;
  return true;
}

void hvostov::meets(const Database& db, size_t id, std::ostream& out)
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

  if (meetCount == 0) {
    out << '\n';
    return;
  }

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

    for (size_t i = 0; i < meetCount; ++i) {
      for (size_t j = i + 1; j < meetCount; ++j) {
        if (result[i].first > result[j].first ||
            (result[i].first == result[j].first && result[i].second > result[j].second)) {
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

void hvostov::commons(const Database& db, size_t id1, size_t id2, std::ostream& out)
{
  if (!findPerson(db, id1) || !findPerson(db, id2)) {
    out << "<INVALID COMMAND>\n";
    return;
  }

  size_t* meets1 = nullptr;
  size_t meets1Count = 0;
  size_t meets1Cap = 8;

  size_t* meets2 = nullptr;
  size_t meets2Count = 0;
  size_t meets2Cap = 8;

  try {
    meets1 = new size_t[meets1Cap];
    meets2 = new size_t[meets2Cap];

    for (size_t i = 0; i < db.meetingsCount; ++i) {
      if (db.meetings[i].id1 == id1 || db.meetings[i].id2 == id1) {
        size_t partner = (db.meetings[i].id1 == id1) ? db.meetings[i].id2 : db.meetings[i].id1;
        if (!arrayContains(meets1, meets1Count, partner)) {
          if (meets1Count >= meets1Cap) {
            meets1 = expandArray(meets1, meets1Count, meets1Cap * 2);
            meets1Cap *= 2;
          }
          meets1[meets1Count++] = partner;
        }
      }

      if (db.meetings[i].id1 == id2 || db.meetings[i].id2 == id2) {
        size_t partner = (db.meetings[i].id1 == id2) ? db.meetings[i].id2 : db.meetings[i].id1;
        if (!arrayContains(meets2, meets2Count, partner)) {
          if (meets2Count >= meets2Cap) {
            meets2 = expandArray(meets2, meets2Count, meets2Cap * 2);
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
      if (arrayContains(meets2, meets2Count, meets1[i])) {
        if (commonCount >= commonCap) {
          common = expandArray(common, commonCount, commonCap * 2);
          commonCap *= 2;
        }
        common[commonCount++] = meets1[i];
      }
    }

    sortArray(common, commonCount);
    printArray(common, commonCount, out);

    delete[] common;
    delete[] meets1;
    delete[] meets2;
  } catch (...) {
    delete[] meets1;
    delete[] meets2;
    throw;
  }
}

void hvostov::less(const Database& db, size_t time, size_t id, std::ostream& out)
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

  if (meetCount == 0) {
    out << '\n';
    return;
  }

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

    for (size_t i = 0; i < meetCount; ++i) {
      for (size_t j = i + 1; j < meetCount; ++j) {
        if (result[i].first > result[j].first ||
            (result[i].first == result[j].first && result[i].second > result[j].second)) {
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

void hvostov::greater(const Database& db, size_t time, size_t id, std::ostream& out)
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

  if (meetCount == 0) {
    out << '\n';
    return;
  }

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

    for (size_t i = 0; i < meetCount; ++i) {
      for (size_t j = i + 1; j < meetCount; ++j) {
        if (result[i].first > result[j].first ||
            (result[i].first == result[j].first && result[i].second > result[j].second)) {
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

void hvostov::outPersons(const Database& db, const std::string& filename)
{
  std::ofstream outFile(filename.c_str(), std::ios::app);
  if (!outFile.is_open()) {
    std::cerr << "Error: cannot open output file\n";
    return;
  }

  bool hasAny = false;
  for (size_t i = 0; i < db.personsCount; ++i) {
    if (db.persons[i].hasDescription) {
      outFile << db.persons[i].id << ' ' << db.persons[i].info << '\n';
      hasAny = true;
    }
  }

  if (!hasAny) {
    outFile << '\n';
  }

  outFile.close();
}
