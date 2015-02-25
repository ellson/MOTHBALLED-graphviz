/**
 * \brief remove overlaps between a set of rectangles.
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

#include <iostream>
#include <cassert>
#include "generate-constraints.h"
#include "solve_VPSC.h"
#include "variable.h"
#include "constraint.h"
#ifdef RECTANGLE_OVERLAP_LOGGING
#include <fstream>
#include "blocks.h"
using std::ios;
using std::ofstream;
using std::endl;
#endif

#define EXTRA_GAP 0.0001

double Rectangle::xBorder = 0;
double Rectangle::yBorder = 0;
/**
 * Takes an array of n rectangles and moves them as little as possible
 * such that rectangles are separated by at least xBorder horizontally
 * and yBorder vertically
 *
 * Works in three passes:
 * 1) removes some overlap horizontally
 * 2) removes remaining overlap vertically
 * 3) a last horizontal pass removes all overlap starting from original
 *    x-positions - this corrects the case where rectangles were moved
 *    too much in the first pass.
 */
void removeRectangleOverlap(int n, Rectangle *rs[], double xBorder,
                            double yBorder) {
  assert(0 <= n);
  try {
    // The extra gap avoids numerical imprecision problems
    Rectangle::setXBorder(xBorder + EXTRA_GAP);
    Rectangle::setYBorder(yBorder + EXTRA_GAP);
    Variable **vs = new Variable *[n];
    for (int i = 0; i < n; i++) {
      vs[i] = new Variable(i, 0, 1);
    }
    Constraint **cs;
    double *oldX = new double[n];
    int m = generateXConstraints(n, rs, vs, cs, true);
    for (int i = 0; i < n; i++) {
      oldX[i] = vs[i]->desiredPosition;
    }
    VPSC vpsc_x(n, vs, m, cs);
#ifdef RECTANGLE_OVERLAP_LOGGING
    ofstream f(LOGFILE, ios::app);
    f << "Calling VPSC: Horizontal pass 1" << endl;
    f.close();
#endif
    vpsc_x.solve();
    for (int i = 0; i < n; i++) {
      rs[i]->moveCentreX(vs[i]->position());
    }
    for (int i = 0; i < m; ++i) {
      delete cs[i];
    }
    delete[] cs;
    // Removing the extra gap here ensures things that were moved to be adjacent
    // to
    // one another above are not considered overlapping
    Rectangle::setXBorder(Rectangle::xBorder - EXTRA_GAP);
    m = generateYConstraints(n, rs, vs, cs);
    VPSC vpsc_y(n, vs, m, cs);
#ifdef RECTANGLE_OVERLAP_LOGGING
    f.open(LOGFILE, ios::app);
    f << "Calling VPSC: Vertical pass" << endl;
    f.close();
#endif
    vpsc_y.solve();
    for (int i = 0; i < n; i++) {
      rs[i]->moveCentreY(vs[i]->position());
      rs[i]->moveCentreX(oldX[i]);
    }
    delete[] oldX;
    for (int i = 0; i < m; ++i) {
      delete cs[i];
    }
    delete[] cs;
    Rectangle::setYBorder(Rectangle::yBorder - EXTRA_GAP);
    m = generateXConstraints(n, rs, vs, cs, false);
    VPSC vpsc_x2(n, vs, m, cs);
#ifdef RECTANGLE_OVERLAP_LOGGING
    f.open(LOGFILE, ios::app);
    f << "Calling VPSC: Horizontal pass 2" << endl;
    f.close();
#endif
    vpsc_x2.solve();
    for (int i = 0; i < n; i++) {
      rs[i]->moveCentreX(vs[i]->position());
      delete vs[i];
    }
    delete[] vs;
    for (int i = 0; i < m; ++i) {
      delete cs[i];
    }
    delete[] cs;
  }
  catch (char const *str) {
    std::cerr << str << std::endl;
    for (int i = 0; i < n; i++) {
      std::cerr << *rs[i] << std::endl;
    }
  }
}
