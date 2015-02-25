/*
   * gd_security.c
   *
   * Implements buffer overflow check routines.
   *
   * Written 2004, Phil Knirsch.
   * Based on netpbm fixes by Alan Cox.
   *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "gd.h"

int overflow2(int a, int b) {
  if (a < 0 || b < 0) {
    fprintf(stderr,
            "gd warning: one parameter to a memory allocation multiplication "
            "is negative, failing operation gracefully\n");
    return 1;
  }
  if (b == 0) return 0;
  if (a > INT_MAX / b) {
    fprintf(stderr,
            "gd warning: product of memory allocation multiplication would "
            "exceed INT_MAX, failing operation gracefully\n");
    return 1;
  }
  return 0;
}
