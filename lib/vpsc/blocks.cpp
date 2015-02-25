/**
 * \brief A block structure defined over the variables
 *
 * A block structure defined over the variables such that each block contains
 * 1 or more variables, with the invariant that all constraints inside a block
 * are satisfied by keeping the variables fixed relative to one another
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

#include "blocks.h"
#include "block.h"
#include "constraint.h"
#ifdef RECTANGLE_OVERLAP_LOGGING
#include <fstream>
using std::ios;
using std::ofstream;
using std::endl;
#endif
using std::set;
using std::vector;
using std::iterator;
using std::list;
using std::copy;

long blockTimeCtr;

Blocks::Blocks(const int n, Variable *vs[]) : vs(vs), nvs(n) {
  blockTimeCtr = 0;
  for (int i = 0; i < nvs; i++) {
    insert(new Block(vs[i]));
  }
}
Blocks::~Blocks(void) {
  blockTimeCtr = 0;
  for (set<Block *>::iterator i = begin(); i != end(); i++) {
    delete *i;
  }
  clear();
}

/**
 * returns a list of variables with total ordering determined by the constraint
 * DAG
 */
list<Variable *> *Blocks::totalOrder() {
  list<Variable *> *order = new list<Variable *>;
  for (int i = 0; i < nvs; i++) {
    vs[i]->visited = false;
  }
  for (int i = 0; i < nvs; i++) {
    if (vs[i]->in.size() == 0) {
      dfsVisit(vs[i], order);
    }
  }
  return order;
}
// Recursive depth first search giving total order by pushing nodes in the DAG
// onto the front of the list when we finish searching them
void Blocks::dfsVisit(Variable *v, list<Variable *> *order) {
  v->visited = true;
  vector<Constraint *>::iterator it = v->out.begin();
  for (; it != v->out.end(); it++) {
    Constraint *c = *it;
    if (!c->right->visited) {
      dfsVisit(c->right, order);
    }
  }
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "  order=" << *v << endl;
#endif
  order->push_front(v);
}
/**
 * Processes incoming constraints, most violated to least, merging with the
 * neighbouring (left) block until no more violated constraints are found
 */
void Blocks::mergeLeft(Block *r) {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "mergeLeft called on " << *r << endl;
#endif
  r->timeStamp = ++blockTimeCtr;
  r->setUpInConstraints();
  Constraint *c = r->findMinInConstraint();
  while (c != NULL && c->slack() < 0) {
#ifdef RECTANGLE_OVERLAP_LOGGING
    f << "mergeLeft on constraint: " << *c << endl;
#endif
    r->deleteMinInConstraint();
    Block *l = c->left->block;
    if (l->in == NULL) l->setUpInConstraints();
    double dist = c->right->offset - c->left->offset - c->gap;
    if (r->vars->size() < l->vars->size()) {
      dist = -dist;
      std::swap(l, r);
    }
    blockTimeCtr++;
    r->merge(l, c, dist);
    r->mergeIn(l);
    r->timeStamp = blockTimeCtr;
    removeBlock(l);
    c = r->findMinInConstraint();
  }
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "merged " << *r << endl;
#endif
}
/**
 * Symmetrical to mergeLeft
 */
void Blocks::mergeRight(Block *l) {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "mergeRight called on " << *l << endl;
#endif
  l->setUpOutConstraints();
  Constraint *c = l->findMinOutConstraint();
  while (c != NULL && c->slack() < 0) {
#ifdef RECTANGLE_OVERLAP_LOGGING
    f << "mergeRight on constraint: " << *c << endl;
#endif
    l->deleteMinOutConstraint();
    Block *r = c->right->block;
    r->setUpOutConstraints();
    double dist = c->left->offset + c->gap - c->right->offset;
    if (l->vars->size() > r->vars->size()) {
      dist = -dist;
      std::swap(l, r);
    }
    l->merge(r, c, dist);
    l->mergeOut(r);
    removeBlock(r);
    c = l->findMinOutConstraint();
  }
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "merged " << *l << endl;
#endif
}
void Blocks::removeBlock(Block *doomed) {
  doomed->deleted = true;
  // erase(doomed);
}
void Blocks::cleanup() {
  vector<Block *> bcopy(begin(), end());
  for (vector<Block *>::iterator i = bcopy.begin(); i != bcopy.end(); i++) {
    Block *b = *i;
    if (b->deleted) {
      erase(b);
      delete b;
    }
  }
}
/**
 * Splits block b across constraint c into two new blocks, l and r (c's left
 * and right sides respectively)
 */
void Blocks::split(Block *b, Block *&l, Block *&r, Constraint *c) {
  b->split(l, r, c);
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "Split left: " << *l << endl;
  f << "Split right: " << *r << endl;
#endif
  r->posn = b->posn;
  r->wposn = r->posn * r->weight;
  mergeLeft(l);
  // r may have been merged!
  r = c->right->block;
  r->wposn = r->desiredWeightedPosition();
  r->posn = r->wposn / r->weight;
  mergeRight(r);
  removeBlock(b);

  insert(l);
  insert(r);
}
/**
 * returns the cost total squared distance of variables from their desired
 * positions
 */
double Blocks::cost() {
  double c = 0;
  for (set<Block *>::iterator i = begin(); i != end(); i++) {
    c += (*i)->cost();
  }
  return c;
}
