#include "types.hpp"
#include "database.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char* argv[])
{
  std::string personsFile = "";
  std::string dataFile = "";

  if (argc < 2 || argc > 3) {
    std::cerr << "Error: invalid number of arguments\n";
    return 1;
  }

  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);

    if (arg.substr(0, 3) == "in:") {
      if (!personsFile.empty()) {
        std::cerr << "Error: duplicate input file argument\n";
        return 1;
      }
      personsFile = arg.substr(3);
    } else if (arg.substr(0, 5) == "data:") {
      if (!dataFile.empty()) {
        std::cerr << "Error: duplicate data file argument\n";
        return 1;
      }
      dataFile = arg.substr(5);
    } else {
      std::cerr << "Error: invalid argument format\n";
      return 1;
    }
  }

  if (dataFile.empty()) {
    std::cerr << "Error: data file is required\n";
    return 1;
  }

  hvostov::Database db;
  hvostov::initDatabase(db);

  try {
    if (!personsFile.empty()) {
      std::ifstream inFile(personsFile);
      if (!inFile.is_open()) {
        std::cerr << "Error: cannot open persons file\n";
        hvostov::freeDatabase(db);
        return 2;
      }

      hvostov::Person person;
      while (inFile >> person) {
        hvostov::addPerson(db, person);
      }
    }

    std::ifstream dataFileStream(dataFile);
    if (!dataFileStream.is_open()) {
      std::cerr << "Error: cannot open data file\n";
      hvostov::freeDatabase(db);
      return 2;
    }

    std::string line;
    while (std::getline(dataFileStream, line)) {
      if (line.empty())
        continue;

      std::istringstream iss(line);
      hvostov::Meeting meeting;

      if (iss >> meeting) {
        hvostov::addMeeting(db, meeting);
      } else {
        std::istringstream iss2(line);
        size_t id1, id2, duration;
        if (iss2 >> id1 >> id2 >> duration) {
          if (id1 != id2) {
            std::cerr << "Error: invalid meeting data\n";
            hvostov::freeDatabase(db);
            return 3;
          }
        } else {
          std::cerr << "Error: invalid meeting data\n";
          hvostov::freeDatabase(db);
          return 3;
        }
      }
    }

    while (std::getline(std::cin, line)) {
      if (line.empty())
        continue;

      std::istringstream iss(line);
      std::string command;
      iss >> command;

      if (command == "anons") {
        hvostov::anons(db, std::cout);
      } else if (command == "deanon") {
        size_t anonId, realId;
        if (iss >> anonId >> realId) {
          if (!hvostov::deanon(db, anonId, realId)) {
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
          while (start < rest.length() && std::isspace(rest[start])) {
            start++;
          }
          rest = rest.substr(start);

          if (!rest.empty() && rest[0] == '"') {
            size_t end = rest.rfind('"');
            if (end != std::string::npos && end > 0) {
              std::string desc = rest.substr(1, end - 1);
              if (!hvostov::redesc(db, id, desc)) {
                hvostov::PersonWithDesc pwd;
                pwd.id = id;
                pwd.info = desc;
                pwd.hasDescription = true;
                hvostov::addPersonWithDesc(db, pwd);
              }
            } else {
              std::cout << "<INVALID COMMAND>\n";
            }
          } else {
            hvostov::desc(db, id, std::cout);
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
            if (!hvostov::redesc(db, id, desc)) {
              hvostov::PersonWithDesc pwd;
              pwd.id = id;
              pwd.info = desc;
              pwd.hasDescription = true;
              hvostov::addPersonWithDesc(db, pwd);
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
          hvostov::meets(db, id, std::cout);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "commons") {
        size_t id1, id2;
        if (iss >> id1 >> id2) {
          hvostov::commons(db, id1, id2, std::cout);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "less") {
        size_t time, id;
        if (iss >> time >> id) {
          hvostov::less(db, time, id, std::cout);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "greater") {
        size_t time, id;
        if (iss >> time >> id) {
          hvostov::greater(db, time, id, std::cout);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else if (command == "out-persons") {
        std::string filename;
        if (iss >> filename) {
          hvostov::outPersons(db, filename);
        } else {
          std::cout << "<INVALID COMMAND>\n";
        }
      } else {
        std::cout << "<INVALID COMMAND>\n";
      }
    }
  } catch (...) {
    hvostov::freeDatabase(db);
    return 1;
  }

  hvostov::freeDatabase(db);
  return 0;
}
