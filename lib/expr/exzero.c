/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

/*
 * Glenn Fowler
 * AT&T Research
 *
 * expression library support
 */

#include "exlib.h"

/*
 * return 0 value for type
 */

Extype_t exzero(int type) {
  Extype_t v;

  switch (type) {
    case FLOATING:
      v.floating = 0.0;
      break;
    case INTEGER:
    case UNSIGNED:
      v.integer = 0;
      break;
    case STRING:
      v.string = expr.nullstring;
      break;
  }
  return v;
}
