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

#include <ast.h>

/*
 * copy t into s, return a pointer to the end of s ('\0')
 */

char *strcopy(register char *s, register const char *t) {
  if (!t) return (s);
  while ((*s++ = *t++))
    ;
  return (--s);
}
