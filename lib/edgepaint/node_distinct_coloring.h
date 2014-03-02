/*************************************************************************
 * Copyright (c) 2014 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#ifndef NODE_DISTINCT_COLORING_H
#define NODE_DISTINCT_COLORING_H

enum {COLOR_RGB, COLOR_GRAY, COLOR_LAB};
enum {ERROR_BAD_LAB_GAMUT_FILE = -10, ERROR_BAD_COLOR_SCHEME = -9};
void node_distinct_coloring(char *color_scheme, char *lightness, int weightedQ, SparseMatrix A, real accuracy, int iter_max, int seed, int *cdim, real **colors, real *color_diff, real *color_diff_sum, int *flag);

#endif
