/*************************************************************************
 * Copyright (c) 2014 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#ifndef LAB_GAMUT_H
#define LAB_GAMUT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  signed char l, a, b;/* l: 0 to 100, a,b: -128 tp 128 */
} char_color_lab;

/*visual studio*/
#if defined(WIN32) && !defined(LAB_GAMUT_EXPORTS)
#define extern __declspec(dllimport)
#endif
/*end visual studio*/

extern const char_color_lab lab_gamut_data[];
extern int lab_gamut_data_size;

#undef extern

#ifdef __cplusplus
}
#endif

#endif
