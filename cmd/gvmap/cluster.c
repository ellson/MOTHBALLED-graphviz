/* $Id$Revision: */
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

#include <stdio.h>
#include <stdlib.h>
#define STANDALONE
#include "general.h"
#include "QuadTree.h"
#include <time.h>
#include "SparseMatrix.h"
#include <getopt.h>
#include "string.h"
#include "make_map.h"
#include "spring_electrical.h"
#include "post_process.h"
#include "overlap.h"
#include "clustering.h"
#include "ingraphs.h"
#include "DotIO.h"
#include "colorutil.h"

int Verbose;
enum {POINTS_ALL = 1, POINTS_LABEL, POINTS_RANDOM};
enum {maxlen = 10000000};
enum {MAX_GRPS = 10000};

#if 0
void *gmalloc(size_t nbytes)
{
    char *rv;
    if (nbytes == 0)
        return NULL;
    rv = malloc(nbytes);
    if (rv == NULL) {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    return rv;
}

void *grealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);
    if (p == NULL && size) {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    return p;
}

#endif

static char* usestr =
"    -C k - generate no more than k clusters (0)\n\
       0 : no limit\n\
    -c k - use clustering method k (0)\n\
       0 : use modularity\n\
       1 : use modularity quality\n\
    -v   - verbose mode\n\
    -?   - print usage\n";

static void usage(char* cmd, int eval)
{
    fprintf(stderr, "Usage: %s <options> graphfile\n", cmd);
    fputs (usestr, stderr);
    exit(eval);
}


static void init(int argc, char *argv[], char **infiles[], int *maxcluster, int *clustering_method){
  char* cmd = argv[0];
  unsigned int c;

  *maxcluster = 0;
  Verbose = 0;

  *clustering_method =  CLUSTERING_MODULARITY;
  while ((c = getopt(argc, argv, ":vC:c:")) != -1) {
    switch (c) {
    case 'c':
      if (!((sscanf(optarg,"%d", clustering_method) > 0) && *clustering_method >= 0)){
	usage(cmd,1);
      }

      break;
    case 'C':
      if (!((sscanf(optarg,"%d",maxcluster) > 0) && *maxcluster >= 0)){
	usage(cmd,1);
      }

      break;
    case 'v':
      Verbose = 1;
      break;
    case '?':
      if (optopt == '?')
	usage(cmd, 0);
      else {
	fprintf(stderr, " option -%c unrecognized - ignored\n",
		optopt);
	usage(cmd, 0);
      }
      break;
    }
  }

  argv += optind;
  argc -= optind;
  if (argc)
    *infiles = argv;
  else
    *infiles = NULL;
}



static Agraph_t *gread(FILE * fp)
{
    return agread(fp, (Agdisc_t *) 0);
}

void clusterGraph (Agraph_t* g, int maxcluster, int clustering_method){
  initDotIO(g);
  attached_clustering(g, maxcluster, clustering_method);
  return;

}

int main(int argc, char *argv[])
{
  Agraph_t *g = 0, *prevg = 0;
  char** infiles;
  int maxcluster;
  int clustering_method;
  ingraph_state ig;

  init(argc, argv, &infiles, &maxcluster, &clustering_method);

  newIngraph (&ig, infiles, gread);

  while ((g = nextGraph (&ig)) != 0) {
    if (prevg) agclose (prevg);
    clusterGraph (g, maxcluster, clustering_method);
    agwrite(g, stdout);
    prevg = g;
  }

  return 0; 
}
