/**
 * \brief Solve an instance of the "Variable Placement with Separation
 * Constraints" problem.
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

#include <cassert>
#include "constraint.h"
#include "block.h"
#include "blocks.h"
#include "solve_VPSC.h"
#include <math.h>
#include <sstream>
#ifdef RECTANGLE_OVERLAP_LOGGING
#include <fstream>
using std::ios;
using std::ofstream;
using std::endl;
#endif

using std::ostringstream;
using std::list;
using std::set;

IncVPSC::IncVPSC(const unsigned n, Variable *vs[], const unsigned m,
                 Constraint *cs[])
    : VPSC(n, vs, m, cs) {
  inactive.assign(cs, cs + m);
  for (ConstraintList::iterator i = inactive.begin(); i != inactive.end();
       i++) {
    (*i)->active = false;
  }
}
VPSC::VPSC(const unsigned n, Variable *vs[], const unsigned m, Constraint *cs[])
    : cs(cs), m(m) {
  bs = new Blocks(n, vs);
#ifdef RECTANGLE_OVERLAP_LOGGING
  printBlocks();
  assert(!constraintGraphIsCyclic(n, vs));
#endif
}
VPSC::~VPSC() { delete bs; }

// useful in debugging
void VPSC::printBlocks() {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  for (set<Block *>::iterator i = bs->begin(); i != bs->end(); i++) {
    Block *b = *i;
    f << "  " << *b << endl;
  }
  for (unsigned i = 0; i < m; i++) {
    f << "  " << *cs[i] << endl;
  }
#endif
}
/**
* Produces a feasible - though not necessarily optimal - solution by
* examining blocks in the partial order defined by the directed acyclic
* graph of constraints. For each block (when processing left to right) we
* maintain the invariant that all constraints to the left of the block
* (incoming constraints) are satisfied. This is done by repeatedly merging
* blocks into bigger blocks across violated constraints (most violated
* first) fixing the position of variables inside blocks relative to one
* another so that constraints internal to the block are satisfied.
*/
void VPSC::satisfy() {
  list<Variable *> *vs = bs->totalOrder();
  for (list<Variable *>::iterator i = vs->begin(); i != vs->end(); i++) {
    Variable *v = *i;
    if (!v->block->deleted) {
      bs->mergeLeft(v->block);
    }
  }
  bs->cleanup();
  for (unsigned i = 0; i < m; i++) {
    if (cs[i]->slack() < -0.0000001) {
#ifdef RECTANGLE_OVERLAP_LOGGING
      ofstream f(LOGFILE, ios::app);
      f << "Error: Unsatisfied constraint: " << *cs[i] << endl;
#endif
      // assert(cs[i]->slack()>-0.0000001);
      throw "Unsatisfied constraint";
    }
  }
  delete vs;
}

void VPSC::refine() {
  bool solved = false;
  // Solve shouldn't loop indefinately
  // ... but just to make sure we limit the number of iterations
  unsigned maxtries = 100;
  while (!solved && maxtries >= 0) {
    solved = true;
    maxtries--;
    for (set<Block *>::const_iterator i = bs->begin(); i != bs->end(); i++) {
      Block *b = *i;
      b->setUpInConstraints();
      b->setUpOutConstraints();
    }
    for (set<Block *>::const_iterator i = bs->begin(); i != bs->end(); i++) {
      Block *b = *i;
      Constraint *c = b->findMinLM();
      if (c != NULL && c->lm < 0) {
#ifdef RECTANGLE_OVERLAP_LOGGING
        ofstream f(LOGFILE, ios::app);
        f << "Split on constraint: " << *c << endl;
#endif
        // Split on c
        Block *l = NULL, *r = NULL;
        bs->split(b, l, r, c);
        bs->cleanup();
        // split alters the block set so we have to restart
        solved = false;
        break;
      }
    }
  }
  for (unsigned i = 0; i < m; i++) {
    if (cs[i]->slack() < -0.0000001) {
      assert(cs[i]->slack() > -0.0000001);
      throw "Unsatisfied constraint";
    }
  }
}
/**
 * Calculate the optimal solution. After using satisfy() to produce a
 * feasible solution, refine() examines each block to see if further
 * refinement is possible by splitting the block. This is done repeatedly
 * until no further improvement is possible.
 */
void VPSC::solve() {
  satisfy();
  refine();
}

void IncVPSC::solve() {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "solve_inc()..." << endl;
#endif
  double lastcost, cost = bs->cost();
  do {
    lastcost = cost;
    satisfy();
    splitBlocks();
    cost = bs->cost();
#ifdef RECTANGLE_OVERLAP_LOGGING
    f << "  cost=" << cost << endl;
#endif
  } while (fabs(lastcost - cost) > 0.0001);
}
/**
 * incremental version of satisfy that allows refinement after blocks are
 * moved.
 *
 *  - move blocks to new positions
 *  - repeatedly merge across most violated constraint until no more
 *    violated constraints exist
 *
 * Note: there is a special case to handle when the most violated constraint
 * is between two variables in the same block.  Then, we must split the block
 * over an active constraint between the two variables.  We choose the
 * constraint with the most negative lagrangian multiplier.
 */
void IncVPSC::satisfy() {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "satisfy_inc()..." << endl;
#endif
  splitBlocks();
  long splitCtr = 0;
  Constraint *v = NULL;
  while (mostViolated(inactive, v) < -0.0000001) {
    assert(!v->active);
    Block *lb = v->left->block, *rb = v->right->block;
    if (lb != rb) {
      lb->merge(rb, v);
    } else {
      if (splitCtr++ > 10000) {
        throw "Cycle Error!";
      }
      // constraint is within block, need to split first
      inactive.push_back(lb->splitBetween(v->left, v->right, lb, rb));
      lb->merge(rb, v);
      bs->insert(lb);
    }
  }
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "  finished merges." << endl;
#endif
  bs->cleanup();
  for (unsigned i = 0; i < m; i++) {
    v = cs[i];
    if (v->slack() < -0.0000001) {
      // assert(cs[i]->slack()>-0.0000001);
      ostringstream s;
      s << "Unsatisfied constraint: " << *v;
      throw s.str().c_str();
    }
  }
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "  finished cleanup." << endl;
  printBlocks();
#endif
}
void IncVPSC::moveBlocks() {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "moveBlocks()..." << endl;
#endif
  for (set<Block *>::const_iterator i(bs->begin()); i != bs->end(); i++) {
    Block *b = *i;
    b->wposn = b->desiredWeightedPosition();
    b->posn = b->wposn / b->weight;
  }
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "  moved blocks." << endl;
#endif
}
void IncVPSC::splitBlocks() {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
#endif
  moveBlocks();
  splitCnt = 0;
  // Split each block if necessary on min LM
  for (set<Block *>::const_iterator i(bs->begin()); i != bs->end(); i++) {
    Block *b = *i;
    Constraint *v = b->findMinLM();
    if (v != NULL && v->lm < -0.0000001) {
#ifdef RECTANGLE_OVERLAP_LOGGING
      f << "    found split point: " << *v << " lm=" << v->lm << endl;
#endif
      splitCnt++;
      Block *b = v->left->block, *l = NULL, *r = NULL;
      assert(v->left->block == v->right->block);
      double pos = b->posn;
      b->split(l, r, v);
      l->posn = r->posn = pos;
      l->wposn = l->posn * l->weight;
      r->wposn = r->posn * r->weight;
      bs->insert(l);
      bs->insert(r);
      b->deleted = true;
      inactive.push_back(v);
#ifdef RECTANGLE_OVERLAP_LOGGING
      f << "  new blocks: " << *l << " and " << *r << endl;
#endif
    }
  }
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "  finished splits." << endl;
#endif
  bs->cleanup();
}

/**
 * Scan constraint list for the most violated constraint, or the first equality
 * constraint
 */
double IncVPSC::mostViolated(ConstraintList &l, Constraint *&v) {
  double minSlack = DBL_MAX;
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "Looking for most violated..." << endl;
#endif
  ConstraintList::iterator end = l.end();
  ConstraintList::iterator deletePoint = end;
  for (ConstraintList::iterator i = l.begin(); i != end; i++) {
    Constraint *c = *i;
    double slack = c->slack();
    if (c->equality || slack < minSlack) {
      minSlack = slack;
      v = c;
      deletePoint = i;
      if (c->equality) break;
    }
  }
  // Because the constraint list is not order dependent we just
  // move the last element over the deletePoint and resize
  // downwards.  There is always at least 1 element in the
  // vector because of search.
  if (deletePoint != end && minSlack < -0.0000001) {
    *deletePoint = l[l.size() - 1];
    l.resize(l.size() - 1);
  }
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "  most violated is: " << *v << endl;
#endif
  return minSlack;
}

#include <map>
using std::map;
using std::vector;
struct node {
  set<node *> in;
  set<node *> out;
};
// useful in debugging - cycles would be BAD
bool VPSC::constraintGraphIsCyclic(const unsigned n, Variable *vs[]) {
  map<Variable *, node *> varmap;
  vector<node *> graph;
  for (unsigned i = 0; i < n; i++) {
    node *u = new node;
    graph.push_back(u);
    varmap[vs[i]] = u;
  }
  for (unsigned i = 0; i < n; i++) {
    for (vector<Constraint *>::iterator c = vs[i]->in.begin();
         c != vs[i]->in.end(); c++) {
      Variable *l = (*c)->left;
      varmap[vs[i]]->in.insert(varmap[l]);
    }

    for (vector<Constraint *>::iterator c = vs[i]->out.begin();
         c != vs[i]->out.end(); c++) {
      Variable *r = (*c)->right;
      varmap[vs[i]]->out.insert(varmap[r]);
    }
  }
  while (graph.size() > 0) {
    node *u = NULL;
    vector<node *>::iterator i = graph.begin();
    for (; i != graph.end(); i++) {
      u = *i;
      if (u->in.size() == 0) {
        break;
      }
    }
    if (i == graph.end() && graph.size() > 0) {
      // cycle found!
      return true;
    } else {
      graph.erase(i);
      for (set<node *>::iterator j = u->out.begin(); j != u->out.end(); j++) {
        node *v = *j;
        v->in.erase(u);
      }
      delete u;
    }
  }
  for (unsigned i = 0; i < graph.size(); i++) {
    delete graph[i];
  }
  return false;
}

// useful in debugging - cycles would be BAD
bool VPSC::blockGraphIsCyclic() {
  map<Block *, node *> bmap;
  vector<node *> graph;
  for (set<Block *>::const_iterator i = bs->begin(); i != bs->end(); i++) {
    Block *b = *i;
    node *u = new node;
    graph.push_back(u);
    bmap[b] = u;
  }
  for (set<Block *>::const_iterator i = bs->begin(); i != bs->end(); i++) {
    Block *b = *i;
    b->setUpInConstraints();
    Constraint *c = b->findMinInConstraint();
    while (c != NULL) {
      Block *l = c->left->block;
      bmap[b]->in.insert(bmap[l]);
      b->deleteMinInConstraint();
      c = b->findMinInConstraint();
    }

    b->setUpOutConstraints();
    c = b->findMinOutConstraint();
    while (c != NULL) {
      Block *r = c->right->block;
      bmap[b]->out.insert(bmap[r]);
      b->deleteMinOutConstraint();
      c = b->findMinOutConstraint();
    }
  }
  while (graph.size() > 0) {
    node *u = NULL;
    vector<node *>::iterator i = graph.begin();
    for (; i != graph.end(); i++) {
      u = *i;
      if (u->in.size() == 0) {
        break;
      }
    }
    if (i == graph.end() && graph.size() > 0) {
      // cycle found!
      return true;
    } else {
      graph.erase(i);
      for (set<node *>::iterator j = u->out.begin(); j != u->out.end(); j++) {
        node *v = *j;
        v->in.erase(u);
      }
      delete u;
    }
  }
  for (unsigned i = 0; i < graph.size(); i++) {
    delete graph[i];
  }
  return false;
}
