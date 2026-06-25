#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <istream>
#include <string>

namespace hvostov {
  struct Person {
    size_t id;
    std::string info;
  };

  struct PersonWithDesc {
    size_t id;
    std::string info;
    bool hasDescription;
  };

  struct Meeting {
    size_t id1;
    size_t id2;
    size_t duration;
  };

  std::istream& operator>>(std::istream& in, Person& dest);
  std::ostream& operator<<(std::ostream& out, const Person& dest);
  std::istream& operator>>(std::istream& in, Meeting& dest);
}

#endif
