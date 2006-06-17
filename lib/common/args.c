/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

/* FIXME
 * This is an ugly mess.
 *
 * Args should be made independent of layout engine and arg values
 * should be stored in gvc or gvc->job.   All globals should be eliminated.
 *
 * Needs to be fixed before layout engines can be plugins.
 */

#include <ctype.h>
#include "render.h"
#include "tlayout.h"

/* Default layout values, possibly set via command line; -1 indicates unset */
fdpParms_t fdp_parms_default = {
    1,                          /* useGrid */
    1,                          /* useNew */
    -1,                         /* numIters */
    50,                         /* unscaled */
    0.0,                        /* C */
    1.0,                        /* Tfact */
    -1.0,                       /* K - set in initParams; used in init_edge */
    -1.0,                       /* T0 */
};


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
      case 'x' : Reduce = true; break;
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
      case 'm' : MemTest = true; break;
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
config_extra_args(GVC_t *gvc, int argc, char** argv)
{
  char** p = argv+1;
  int    i;
  char*  arg;
  int    cnt = 1;

  for (i = 1; i < argc; i++) {
    arg = argv[i];
    if (arg && *arg == '-') {
      switch (arg[1]) {
      case 'v':
	gvc->common.verbose = 1;
	if (isdigit(arg[2]))
	  gvc->common.verbose = atoi(&arg[2]);
        break;
      case 'c' :
          gvc->common.config = true; break;
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
    fdp_parms.useGrid = 0;
    break;
  case 'O' :
    fdp_parms.useNew = 0;
    break;
  case 'n' :
    if (setInt (&fdp_parms.numIters, arg)) return 1;
    break;
  case 'U' :
    if (setInt (&fdp_parms.unscaled, arg)) return 1;
    break;
  case 'C' :
    if (setDouble (&fdp_parms.C, arg)) return 1;
    break;
  case 'T' :
    if (*arg == '*') {
      if (setDouble (&fdp_parms.Tfact, arg+1)) return 1;
    }
    else {
      if (setDouble (&fdp_parms.T0, arg)) return 1;
    }
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

  fdp_parms = fdp_parms_default;

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

int gvParseArgs(GVC_t *gvc, int argc, char** argv)
{
    argc = neato_extra_args(gvc, argc, argv);
    argc = fdp_extra_args(gvc, argc, argv);
    argc = memtest_extra_args(gvc, argc, argv);
    argc = config_extra_args(gvc, argc, argv);
    dotneato_args_initialize(gvc, argc, argv);
    if (Verbose)
	gvplugin_write_status(gvc);
    return 0;
}
