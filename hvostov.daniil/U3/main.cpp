#include <database.hpp>
#include "types.hpp"
#include "database.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char* argv[])
{
  std::string personsFile = "";
  std::string* dataFiles = nullptr;
  size_t dataFilesCount = 0;

  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);

    if (arg.substr(0, 3) == "in:") {
      if (!personsFile.empty()) {
        std::cerr << "Error: duplicate input file argument\n";
        delete[] dataFiles;
        return 1;
      }
      personsFile = arg.substr(3);
    } else if (arg.substr(0, 5) == "data:") {
      std::string* tmp = new std::string[dataFilesCount + 1];
      for (size_t j = 0; j < dataFilesCount; ++j) {
        tmp[j] = dataFiles[j];
      }
      tmp[dataFilesCount] = arg.substr(5);
      delete[] dataFiles;
      dataFiles = tmp;
      dataFilesCount++;
    } else {
      std::cerr << "Error: invalid argument format\n";
      delete[] dataFiles;
      return 1;
    }
  }

  hvostov::DatabaseU3 db;
  hvostov::initDatabaseU3(db);

  try {
    if (!personsFile.empty()) {
      std::ifstream inFile(personsFile.c_str());
      if (!inFile.is_open()) {
        std::cerr << "Error: cannot open persons file\n";
        hvostov::freeDatabaseU3(db);
        delete[] dataFiles;
        return 2;
      }

      hvostov::Person person;
      while (inFile >> person) {
        hvostov::addPerson(db.base, person);
      }
    }

    for (size_t f = 0; f < dataFilesCount; ++f) {
      std::ifstream dataFileStream(dataFiles[f].c_str());
      if (!dataFileStream.is_open()) {
        std::cerr << "Error: cannot open data file\n";
        hvostov::freeDatabaseU3(db);
        delete[] dataFiles;
        return 2;
      }

      std::string line;
      while (std::getline(dataFileStream, line)) {
        if (line.empty())
          continue;

        std::istringstream iss(line);
        hvostov::MeetingWithDate meeting;

        if (iss >> meeting) {
          hvostov::addMeetingU3(db, meeting);
        } else {
          std::istringstream iss2(line);
          hvostov::Date date;
          size_t id1, id2, duration;
          if (iss2 >> date >> id1 >> id2 >> duration) {
            if (id1 != id2) {
              std::cerr << "Error: invalid meeting data\n";
              hvostov::freeDatabaseU3(db);
              delete[] dataFiles;
              return 3;
            }
          } else {
            std::cerr << "Error: invalid meeting data\n";
            hvostov::freeDatabaseU3(db);
            delete[] dataFiles;
            return 3;
          }
        }
      }
    }

    hvostov::initDateRange(db);

    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty())
        continue;

      std::istringstream iss(line);
      std::string command;
      iss >> command;

      if (command == "range") {
        hvostov::range(db, std::cout);
      } else if (command == "after") {
        hvostov::Date date;
        if (iss >> date) {
          if (!hvostov::after(db, date)) {
            std::cout << "<INVALID COMMAND>\n";
          }
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "before") {
        hvostov::Date date;
        if (iss >> date) {
          if (!hvostov::before(db, date)) {
            std::cout << "<INVALID COMMAND>\n";
          }
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "pop-range") {
        if (!hvostov::popRange(db)) {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "anons") {
        hvostov::anons(db.base, std::cout);
      } else if (command == "deanon") {
        size_t anonId, realId;
        if (iss >> anonId >> realId) {
          if (!hvostov::deanon(db.base, anonId, realId)) {
            std::cout << "<INVALID COMMAND>\n";
          }
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "desc") {
        size_t id;
        if (iss >> id) {
          std::string rest;
          std::getline(iss, rest);
          size_t start = 0;
          while (start < rest.length() && std::isspace(rest[start]))
            start++;
          rest = rest.substr(start);

          if (!rest.empty() && rest[0] == '"') {
            size_t end = rest.rfind('"');
            if (end != std::string::npos && end > 0) {
              std::string desc = rest.substr(1, end - 1);
              if (!hvostov::redesc(db.base, id, desc)) {
                hvostov::PersonWithDesc pwd = {id, desc, true};
                hvostov::addPersonWithDesc(db.base, pwd);
              }
            } else {
              std::cout << "<INVALID COMMAND>\n";
            }
          } else {
            hvostov::desc(db.base, id, std::cout);
          }
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "redesc") {
        size_t id;
        if (iss >> id) {
          std::string rest;
          std::getline(iss, rest);
          size_t start = rest.find('"');
          size_t end = rest.rfind('"');
          if (start != std::string::npos && end != std::string::npos && start < end) {
            std::string desc = rest.substr(start + 1, end - start - 1);
            if (!hvostov::redesc(db.base, id, desc)) {
              hvostov::PersonWithDesc pwd = {id, desc, true};
              hvostov::addPersonWithDesc(db.base, pwd);
            }
          } else {
            std::cout << "<INVALID COMMAND>\n";
          }
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "meet" || command == "meets") {
        size_t id;
        if (iss >> id) {
          hvostov::meets(db.base, id, std::cout);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "commons") {
        size_t id1, id2;
        if (iss >> id1 >> id2) {
          hvostov::commons(db.base, id1, id2, std::cout);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "less") {
        size_t time, id;
        if (iss >> time >> id) {
          hvostov::less(db.base, time, id, std::cout);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "greater") {
        size_t time, id;
        if (iss >> time >> id) {
          hvostov::greater(db.base, time, id, std::cout);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "out-persons") {
        std::string filename;
        if (iss >> filename) {
          hvostov::outPersons(db.base, filename);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else {
        std::cout << "<INVALID COMMAND>\n";
      }
    }
  } catch (...) {
    hvostov::freeDatabaseU3(db);
    delete[] dataFiles;
    return 1;
  }

  hvostov::freeDatabaseU3(db);
  delete[] dataFiles;
  return 0;
}
