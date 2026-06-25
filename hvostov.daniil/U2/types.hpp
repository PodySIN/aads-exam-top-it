#ifndef U2_TYPES_HPP
#define U2_TYPES_HPP

#include <types.hpp>
#include <iostream>

namespace hvostov {
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

  struct Database {
    PersonWithDesc* persons;
    size_t personsCount;
    size_t personsCapacity;
    Meeting* meetings;
    size_t meetingsCount;
    size_t meetingsCapacity;
  };

  std::istream& operator>>(std::istream& in, Meeting& dest);
}

#endif
