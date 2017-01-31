/**
 * \brief A constraint determines a minimum or exact spacing required between
 * two variables.
 *
 * Authors:
 *   Tim Dwyer <tgdwyer@gmail.com>
 *
 * Copyright (C) 2005 Authors
 *
 * Released under GNU LGPL.  Read the file 'COPYING' for more information.
 */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
#endif

#include "constraint.h"
#include <cassert>
namespace vpsc {
Constraint::Constraint(Variable *left, Variable *right, double gap, bool equality)
    : left(left), right(right), gap(gap), lm(0), timeStamp(0), active(false), visited(false), equality(equality) {
  left->out.push_back(this);
  right->in.push_back(this);
}
Constraint::~Constraint() {
  Constraints::iterator i;

  for (i = left->out.begin(); i != left->out.end(); ++i) {
    if (*i == this)
      break;
  }

  left->out.erase(i);

  for (i = right->in.begin(); i != right->in.end(); ++i) {
    if (*i == this)
      break;
  }

  right->in.erase(i);
}
std::ostream &operator<<(std::ostream &os, const Constraint &c) {
  const char *type = c.equality ? "=" : "<=";
  os << *c.left << "+" << c.gap << type << *c.right << "(" << c.slack() << ")" << (c.active ? "-active" : "");
  return os;
}
}
