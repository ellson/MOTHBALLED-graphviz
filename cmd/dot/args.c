/*
    This software may only be used by you under license from AT&T Corp.
    ("AT&T").  A copy of AT&T's Source Code Agreement is available at
    AT&T's Internet website having the URL:
    <http://www.research.att.com/sw/tools/graphviz/license/source.html>
    If you received this software without first entering into a license
    with AT&T, you have an infringing copy of this software and cannot use
    it without violating AT&T's intellectual property rights.
*/

/* FIXME
 * This is an ugly mess.
 *
 * Args should be made independent of layout engine and arg values
 * should be stored in gvc or gvc->job.   All globals should be eliminated.
 *
 * Needs to be fixed before layout engines can be plugins.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dot.h"
#include "neato.h"
#include "fdp.h"
#include "tlayout.h"
#include "circular.h"
#include "circle.h"

static int
neato_extra_args(GVC_t *gvc, int argc, char** argv)
{
  char** p = argv+1;
  int    i;
  char*  arg;
  int    cnt = 1;

  for (i = 1; i < argc; i++) {
    arg = argv[i];
    if (arg && *arg == '-') {
      switch (arg[1]) {
      case 'x' : Reduce = TRUE; break;
      case 'n':
        if (arg[2]) {
          Nop = atoi(arg+2);
          if (Nop <= 0) {
            fprintf (stderr, "Invalid parameter \"%s\" for -n flag\n", arg+2);
            dotneato_usage (1);
          }
        }
        else Nop = 1;
        break;
      default :
        cnt++;
        if (*p != arg) *p = arg;
        p++;
        break;
      }
    }
    else {
      cnt++;
      if (*p != arg) *p = arg;
      p++;
    }
  }
  *p = 0;
  return cnt;
}

static int
memtest_extra_args(GVC_t *gvc, int argc, char** argv)
{
  char** p = argv+1;
  int    i;
  char*  arg;
  int    cnt = 1;

  for (i = 1; i < argc; i++) {
    arg = argv[i];
    if (arg && *arg == '-') {
      switch (arg[1]) {
      case 'm' : MemTest = TRUE; break;
      default :
        cnt++;
        if (*p != arg) *p = arg;
        p++;
        break;
      }
    }
    else {
      cnt++;
      if (*p != arg) *p = arg;
      p++;
    }
  }
  *p = 0;
  return cnt;
}

/* setDouble:
 * If arg is an double, value is stored in v
 * and functions returns 0; otherwise, returns 1.
 */
static int
setDouble (double* v, char* arg)
{
  char*    p;
  double   d;

  d = strtod(arg,&p);
  if (p == arg) {
    agerr (AGERR, "bad value in flag -L%s - ignored\n", arg-1);
    return 1;
  }
  *v = d;
  return 0;
}

/* setInt:
 * If arg is an integer, value is stored in v
 * and functions returns 0; otherwise, returns 1.
 */
static int
setInt (int* v, char* arg)
{
  char*    p;
  int      i;

  i = (int)strtol(arg,&p,10);
  if (p == arg) {
    agerr (AGERR, "bad value in flag -L%s - ignored\n", arg-1);
    return 1;
  }
  *v = i;
  return 0;
}

/* setAttr:
 * Actions for fdp specific flags
 */
static int
setAttr (char* arg)
{
  switch (*arg++) {
  case 'g' :
    fdp_tvals.useGrid = 0;
    break;
  case 'O' :
    fdp_tvals.useNew = 0;
    break;
  case 'n' :
    if (setInt (&fdp_numIters, arg)) return 1;
    break;
  case 't' :
    if (setInt (&fdp_Tries, arg)) return 1;
    break;
  case 'M' :
    if (setInt (&fdp_tvals.maxIter, arg)) return 1;
    break;
  case 'U' :
    if (setInt (&fdp_tvals.unscaled, arg)) return 1;
    break;
  case 'C' :
    if (setDouble (&fdp_tvals.C, arg)) return 1;
    break;
#if 0
  case 's' :
    if (setDouble (&Scale, arg)) return 1;
    break;
#endif
  case 'K' :
    if (setDouble (&fdp_K, arg)) return 1;
    break;
  case 'T' :
    if (*arg == '*') {
      if (setDouble (&fdp_tvals.Tfact, arg+1)) return 1;
    }
    else {
      if (setDouble (&fdp_T0, arg)) return 1;
    }
    break;
  case 'S' :
    fdp_seedarg = arg;
    break;
  default :
    agerr (AGWARN, "unknown flag -L%s - ignored\n", arg-1);
    break;
  }
  return 0;
}

/* fdp_extra_args:
 * Handle fdp specific arguments.
 * These have the form -L<name>=<value>.
 */
static int
fdp_extra_args (GVC_t *gvc, int argc, char** argv)
{
  char** p = argv+1;
  int    i;
  char*  arg;
  int    cnt = 1;

  fdp_numIters = -1;
  fdp_K = -1.0;
  fdp_T0 = -1.0;
  fdp_seedarg = NULL;
  fdp_smode = seed_unset;
  
  for (i = 1; i < argc; i++) {
    arg = argv[i];
    if (arg && (*arg == '-') && (*(arg+1) == 'L')) {
      if (setAttr (arg+2)) dotneato_usage(1);
    }
    else {
      cnt++;
      if (*p != arg) *p = arg;
      p++;
    }
  }
  *p = 0;
  return cnt;
}

void parse_args(GVC_t *gvc, int argc, char** argv)
{
        neato_extra_args(gvc, argc, argv);
        fdp_extra_args(gvc, argc, argv);
        memtest_extra_args(gvc, argc, argv);
        dotneato_initialize(gvc, argc, argv);
}
