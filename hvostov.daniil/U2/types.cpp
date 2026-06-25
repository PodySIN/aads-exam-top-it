#include "types.hpp"

std::istream& hvostov::operator>>(std::istream& in, Meeting& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry) {
    return in;
  }

  Meeting input;

  if (!(in >> input.id1 >> input.id2 >> input.duration)) {
    return in;
  }

  if (input.id1 == input.id2) {
    in.setstate(std::ios::failbit);
    return in;
  }

  dest = input;
  return in;
}
