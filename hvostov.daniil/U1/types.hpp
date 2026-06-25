#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <istream>

namespace hvostov {
  struct Person {
    size_t id;
    std::string info;
  };

  std::istream& operator>>(std::istream& in, Person& dest);
  std::ostream& operator<<(std::ostream& out, const Person& dest);

}
#endif
