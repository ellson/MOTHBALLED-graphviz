/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**
 * \brief Functions to automatically generate constraints for the rectangular
 * node overlap removal problem.
 *
 * Authors:
 *   Tim Dwyer <tgdwyer@gmail.com>
 *
 * Copyright (C) 2005 Authors
 *
 * This version is released under the CPL (Common Public License) with
 * the Graphviz distribution.
 * A version is also available under the LGPL as part of the Adaptagrams
 * project: http://sourceforge.net/projects/adaptagrams.
 * If you make improvements or bug fixes to this code it would be much
 * appreciated if you could also contribute those changes back to the
 * Adaptagrams repository.
 */
#ifndef SEEN_REMOVEOVERLAP_GENERATE_CONSTRAINTS_H
#define SEEN_REMOVEOVERLAP_GENERATE_CONSTRAINTS_H
#include <iostream>

class Rectangle {
  friend std::ostream& operator<<(std::ostream& os, const Rectangle& r);

 public:
  static double xBorder, yBorder;
  Rectangle(double x, double X, double y, double Y);
  double getMaxX() const { return maxX + xBorder; }
  double getMaxY() const { return maxY + yBorder; }
  double getMinX() const { return minX; }
  double getMinY() const { return minY; }
  double getMinD(unsigned const d) const {
    return (d == 0 ? getMinX() : getMinY());
  }
  double getMaxD(unsigned const d) const {
    return (d == 0 ? getMaxX() : getMaxY());
  }
  double getCentreX() const { return minX + width() / 2.0; }
  double getCentreY() const { return minY + height() / 2.0; }
  double width() const { return getMaxX() - minX; }
  double height() const { return getMaxY() - minY; }
  static void setXBorder(double x) { xBorder = x; }
  static void setYBorder(double y) { yBorder = y; }
  void moveCentreX(double x) { moveMinX(x - width() / 2.0); }
  void moveCentreY(double y) { moveMinY(y - height() / 2.0); }
  void moveMinX(double x) {
    maxX = x + width() - xBorder;
    minX = x;
  }
  void moveMinY(double y) {
    maxY = y + height() - yBorder;
    minY = y;
  }
  inline double overlapX(Rectangle* r) const {
    if (getCentreX() <= r->getCentreX() && r->minX < getMaxX())
      return getMaxX() - r->minX;
    if (r->getCentreX() <= getCentreX() && minX < r->getMaxX())
      return r->getMaxX() - minX;
    return 0;
  }
  inline double overlapY(Rectangle* r) const {
    if (getCentreY() <= r->getCentreY() && r->minY < getMaxY())
      return getMaxY() - r->minY;
    if (r->getCentreY() <= getCentreY() && minY < r->getMaxY())
      return r->getMaxY() - minY;
    return 0;
  }

 private:
  double minX, maxX, minY, maxY;
};

class Variable;
class Constraint;

// returns number of constraints generated
int generateXConstraints(const int n, Rectangle** rs, Variable** vars,
                         Constraint**& cs, const bool useNeighbourLists);
int generateYConstraints(const int n, Rectangle** rs, Variable** vars,
                         Constraint**& cs);

#endif  // SEEN_REMOVEOVERLAP_GENERATE_CONSTRAINTS_H
