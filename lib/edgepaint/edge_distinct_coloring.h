/*************************************************************************
 * Copyright (c) 2014 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#ifndef EDGE_DISTINCT_COLORING_H
#define EDGE_DISTINCT_COLORING_H

Agraph_t* edge_distinct_coloring(char *color_scheme, char *lightness, Agraph_t* g, real angle, real accuracy, int check_edges_with_same_endpoint, int seed);

#endif
