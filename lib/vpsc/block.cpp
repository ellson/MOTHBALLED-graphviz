/**
 * \brief A block is a group of variables that must be moved together to improve
 * the goal function without violating already active constraints.
 * The variables in a block are spanned by a tree of active constraints.
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
#include "pairingheap/PairingHeap.h"
#include "constraint.h"
#include "block.h"
#include "blocks.h"
#ifdef RECTANGLE_OVERLAP_LOGGING
#include <fstream>
using std::ios;
using std::ofstream;
using std::endl;
#endif
using std::vector;

typedef vector<Constraint *>::iterator Cit;

void Block::addVariable(Variable *v) {
  v->block = this;
  vars->push_back(v);
  weight += v->weight;
  wposn += v->weight * (v->desiredPosition - v->offset);
  posn = wposn / weight;
}
Block::Block(Variable *v) {
  timeStamp = 0;
  posn = weight = wposn = 0;
  in = NULL;
  out = NULL;
  deleted = false;
  vars = new vector<Variable *>;
  if (v != NULL) {
    v->offset = 0;
    addVariable(v);
  }
}

double Block::desiredWeightedPosition() {
  double wp = 0;
  for (vector<Variable *>::iterator v = vars->begin(); v != vars->end(); v++) {
    wp += ((*v)->desiredPosition - (*v)->offset) * (*v)->weight;
  }
  return wp;
}
Block::~Block(void) {
  delete vars;
  delete in;
  delete out;
}
void Block::setUpInConstraints() { setUpConstraintHeap(in, true); }
void Block::setUpOutConstraints() { setUpConstraintHeap(out, false); }
void Block::setUpConstraintHeap(PairingHeap<Constraint *> *&h, bool in) {
  delete h;
  h = new PairingHeap<Constraint *>(&compareConstraints);
  for (vector<Variable *>::iterator i = vars->begin(); i != vars->end(); i++) {
    Variable *v = *i;
    vector<Constraint *> *cs = in ? &(v->in) : &(v->out);
    for (vector<Constraint *>::iterator j = cs->begin(); j != cs->end(); j++) {
      Constraint *c = *j;
      c->timeStamp = blockTimeCtr;
      if (c->left->block != this && in || c->right->block != this && !in) {
        h->insert(c);
      }
    }
  }
}
void Block::merge(Block *b, Constraint *c) {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "  merging on: " << *c << ",c->left->offset=" << c->left->offset
    << ",c->right->offset=" << c->right->offset << endl;
#endif
  double dist = c->right->offset - c->left->offset - c->gap;
  Block *l = c->left->block;
  Block *r = c->right->block;
  if (vars->size() < b->vars->size()) {
    r->merge(l, c, dist);
  } else {
    l->merge(r, c, -dist);
  }
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "  merged block=" << (b->deleted ? *this : *b) << endl;
#endif
}
/**
 * Merges b into this block across c.  Can be either a
 * right merge or a left merge
 * @param b block to merge into this
 * @param c constraint being merged
 * @param distance separation required to satisfy c
 */
void Block::merge(Block *b, Constraint *c, double dist) {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "    merging: " << *b << "dist=" << dist << endl;
#endif
  c->active = true;
  wposn += b->wposn - dist * b->weight;
  weight += b->weight;
  posn = wposn / weight;
  for (vector<Variable *>::iterator i = b->vars->begin(); i != b->vars->end();
       i++) {
    Variable *v = *i;
    v->block = this;
    v->offset += dist;
    vars->push_back(v);
  }
  b->deleted = true;
}

void Block::mergeIn(Block *b) {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "  merging constraint heaps... " << endl;
#endif
  // We check the top of the heaps to remove possible internal constraints
  findMinInConstraint();
  b->findMinInConstraint();
  in->merge(b->in);
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "  merged heap: " << *in << endl;
#endif
}
void Block::mergeOut(Block *b) {
  findMinOutConstraint();
  b->findMinOutConstraint();
  out->merge(b->out);
}
Constraint *Block::findMinInConstraint() {
  Constraint *v = NULL;
  vector<Constraint *> outOfDate;
  while (!in->isEmpty()) {
    v = in->findMin();
    Block *lb = v->left->block;
    Block *rb = v->right->block;
// rb may not be this if called between merge and mergeIn
#ifdef RECTANGLE_OVERLAP_LOGGING
    ofstream f(LOGFILE, ios::app);
    f << "  checking constraint ... " << *v;
    f << "    timestamps: left=" << lb->timeStamp << " right=" << rb->timeStamp
      << " constraint=" << v->timeStamp << endl;
#endif
    if (lb == rb) {
// constraint has been merged into the same block
#ifdef RECTANGLE_OVERLAP_LOGGING
      if (v->slack() < 0) {
        f << "  violated internal constraint found! " << *v << endl;
        f << "     lb=" << *lb << endl;
        f << "     rb=" << *rb << endl;
      }
#endif
      in->deleteMin();
#ifdef RECTANGLE_OVERLAP_LOGGING
      f << " ... skipping internal constraint" << endl;
#endif
    } else if (v->timeStamp < lb->timeStamp) {
      // block at other end of constraint has been moved since this
      in->deleteMin();
      outOfDate.push_back(v);
#ifdef RECTANGLE_OVERLAP_LOGGING
      f << "    reinserting out of date (reinsert later)" << endl;
#endif
    } else {
      break;
    }
  }
  for (vector<Constraint *>::iterator i = outOfDate.begin();
       i != outOfDate.end(); i++) {
    v = *i;
    v->timeStamp = blockTimeCtr;
    in->insert(v);
  }
  if (in->isEmpty()) {
    v = NULL;
  } else {
    v = in->findMin();
  }
  return v;
}
Constraint *Block::findMinOutConstraint() {
  if (out->isEmpty()) return NULL;
  Constraint *v = out->findMin();
  while (v->left->block == v->right->block) {
    out->deleteMin();
    if (out->isEmpty()) return NULL;
    v = out->findMin();
  }
  return v;
}
void Block::deleteMinInConstraint() {
  in->deleteMin();
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "deleteMinInConstraint... " << endl;
  f << "  result: " << *in << endl;
#endif
}
void Block::deleteMinOutConstraint() { out->deleteMin(); }
inline bool Block::canFollowLeft(Constraint *c, Variable *last) {
  return c->left->block == this && c->active && last != c->left;
}
inline bool Block::canFollowRight(Constraint *c, Variable *last) {
  return c->right->block == this && c->active && last != c->right;
}

// computes the derivative of v and the lagrange multipliers
// of v's out constraints (as the recursive sum of those below.
// Does not backtrack over u.
// also records the constraint with minimum lagrange multiplier
// in min_lm
double Block::compute_dfdv(Variable *v, Variable *u, Constraint *&min_lm) {
  double dfdv = v->weight * (v->position() - v->desiredPosition);
  for (vector<Constraint *>::iterator it = v->out.begin(); it != v->out.end();
       it++) {
    Constraint *c = *it;
    if (canFollowRight(c, u)) {
      dfdv += c->lm = compute_dfdv(c->right, v, min_lm);
      if (min_lm == NULL || c->lm < min_lm->lm) min_lm = c;
    }
  }
  for (vector<Constraint *>::iterator it = v->in.begin(); it != v->in.end();
       it++) {
    Constraint *c = *it;
    if (canFollowLeft(c, u)) {
      dfdv -= c->lm = -compute_dfdv(c->left, v, min_lm);
      if (min_lm == NULL || c->lm < min_lm->lm) min_lm = c;
    }
  }
  return dfdv;
}

// computes dfdv for each variable and uses the sum of dfdv on either side of
// the constraint c to compute the lagrangian multiplier lm_c.
// The top level v and r are variables between which we want to find the
// constraint with the smallest lm.
// When we find r we pass NULL to subsequent recursive calls,
// thus r=NULL indicates constraints are not on the shortest path.
// Similarly, m is initially NULL and is only assigned a value if the next
// variable to be visited is r or if a possible min constraint is returned from
// a nested call (rather than NULL).
// Then, the search for the m with minimum lm occurs as we return from
// the recursion (checking only constraints traversed left-to-right
// in order to avoid creating any new violations).
Block::Pair Block::compute_dfdv_between(Variable *r, Variable *v, Variable *u,
                                        Direction dir = NONE,
                                        bool changedDirection = false) {
  double dfdv = v->weight * (v->position() - v->desiredPosition);
  Constraint *m = NULL;
  for (Cit it(v->in.begin()); it != v->in.end(); it++) {
    Constraint *c = *it;
    if (canFollowLeft(c, u)) {
      if (dir == RIGHT) {
        changedDirection = true;
      }
      if (c->left == r) {
        r = NULL;
        m = c;
      }
      Pair p = compute_dfdv_between(r, c->left, v, LEFT, changedDirection);
      dfdv -= c->lm = -p.first;
      if (r && p.second) m = p.second;
    }
  }
  for (Cit it(v->out.begin()); it != v->out.end(); it++) {
    Constraint *c = *it;
    if (canFollowRight(c, u)) {
      if (dir == LEFT) {
        changedDirection = true;
      }
      if (c->right == r) {
        r = NULL;
        m = c;
      }
      Pair p = compute_dfdv_between(r, c->right, v, RIGHT, changedDirection);
      dfdv += c->lm = p.first;
      if (r && p.second)
        m = changedDirection && c->lm < p.second->lm ? c : p.second;
    }
  }
  return Pair(dfdv, m);
}

// resets LMs for all active constraints to 0 by
// traversing active constraint tree starting from v,
// not back tracking over u
void Block::reset_active_lm(Variable *v, Variable *u) {
  for (vector<Constraint *>::iterator it = v->out.begin(); it != v->out.end();
       it++) {
    Constraint *c = *it;
    if (canFollowRight(c, u)) {
      c->lm = 0;
      reset_active_lm(c->right, v);
    }
  }
  for (vector<Constraint *>::iterator it = v->in.begin(); it != v->in.end();
       it++) {
    Constraint *c = *it;
    if (canFollowLeft(c, u)) {
      c->lm = 0;
      reset_active_lm(c->left, v);
    }
  }
}
/**
 * finds the constraint with the minimum lagrange multiplier, that is, the
 * constraint
 * that most wants to split
 */
Constraint *Block::findMinLM() {
  Constraint *min_lm = NULL;
  reset_active_lm(vars->front(), NULL);
  compute_dfdv(vars->front(), NULL, min_lm);
  return min_lm;
}
Constraint *Block::findMinLMBetween(Variable *lv, Variable *rv) {
  Constraint *min_lm = NULL;
  reset_active_lm(vars->front(), NULL);
  min_lm = compute_dfdv_between(rv, lv, NULL).second;
  return min_lm;
}

// populates block b by traversing the active constraint tree adding variables
// as they're
// visited.  Starts from variable v and does not backtrack over variable u.
void Block::populateSplitBlock(Block *b, Variable *v, Variable *u) {
  b->addVariable(v);
  for (vector<Constraint *>::iterator c = v->in.begin(); c != v->in.end();
       c++) {
    if (canFollowLeft(*c, u)) populateSplitBlock(b, (*c)->left, v);
  }
  for (vector<Constraint *>::iterator c = v->out.begin(); c != v->out.end();
       c++) {
    if (canFollowRight(*c, u)) populateSplitBlock(b, (*c)->right, v);
  }
}
/**
 * Block needs to be split because of a violated constraint between vl and vr.
 * We need to search the active constraint tree between l and r and find the
 * constraint
 * with min lagrangrian multiplier and split at that point.
 * Returns the split constraint
 */
Constraint *Block::splitBetween(Variable *vl, Variable *vr, Block *&lb,
                                Block *&rb) {
#ifdef RECTANGLE_OVERLAP_LOGGING
  ofstream f(LOGFILE, ios::app);
  f << "  need to split between: " << *vl << " and " << *vr << endl;
#endif
  Constraint *c = findMinLMBetween(vl, vr);
#ifdef RECTANGLE_OVERLAP_LOGGING
  f << "  going to split on: " << *c << endl;
#endif
  split(lb, rb, c);
  deleted = true;
  return c;
}
/**
 * Creates two new blocks, l and r, and splits this block across constraint c,
 * placing the left subtree of constraints (and associated variables) into l
 * and the right into r.
 */
void Block::split(Block *&l, Block *&r, Constraint *c) {
  c->active = false;
  l = new Block();
  populateSplitBlock(l, c->left, c->right);
  r = new Block();
  populateSplitBlock(r, c->right, c->left);
}

/**
 * Computes the cost (squared euclidean distance from desired positions) of the
 * current positions for variables in this block
 */
double Block::cost() {
  double c = 0;
  for (vector<Variable *>::iterator v = vars->begin(); v != vars->end(); v++) {
    double diff = (*v)->position() - (*v)->desiredPosition;
    c += (*v)->weight * diff * diff;
  }
  return c;
}
ostream &operator<<(ostream &os, const Block &b) {
  os << "Block:";
  for (vector<Variable *>::iterator v = b.vars->begin(); v != b.vars->end();
       v++) {
    os << " " << **v;
  }
  if (b.deleted) {
    os << " Deleted!";
  }
  return os;
}
