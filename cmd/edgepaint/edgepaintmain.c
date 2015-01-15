
/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32 /*dependencies*/
    #pragma comment( lib, "cgraph.lib" )
    #pragma comment( lib, "gvc.lib" )
    #pragma comment( lib, "ingraphs.lib" )
    #pragma comment( lib, "sparse.lib" )
    #pragma comment( lib, "sfdp.lib" )
    #pragma comment( lib, "edgepaintlib.lib" )
    #pragma comment( lib, "neatogen.lib" )
    #pragma comment( lib, "rbtree.lib" )
    #pragma comment( lib, "cdt.lib" )
#endif   /* not WIN32_DLL */

#include <cgraph.h>
#include <agxbuf.h>
#include <ingraphs.h>
#include <pointset.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

#include "general.h"
#include "SparseMatrix.h"
#include "DotIO.h"
#include "node_distinct_coloring.h"
#include "edge_distinct_coloring.h"
#include "color_palette.h"

typedef enum {
	FMT_GV,
	FMT_SIMPLE,
} fmt_t;

typedef struct {
	Agrec_t hdr;
	int idx;
} etoi_t;

#define ED_idx(e) (((etoi_t*)AGDATA(e))->idx)


static char *fname;
static FILE *outfile;

static FILE *openFile(char *name, char *mode, char* cmd)
{
    FILE *fp;
    char *modestr;

	fp = fopen(name, mode);
	if (!fp) {
		if (*mode == 'r')
			modestr = "reading";
		else
			modestr = "writing";
		fprintf(stderr, "%s: could not open file %s for %s\n",
			cmd, name, modestr);
		exit(-1);
	}
	return (fp);
}

static void usage (char* cmd, int eval){
  fprintf(stderr, "Usage: %s <options> gv file with 2D coordinates.\n", cmd);
  fprintf(stderr, "Find a color assignment of the edges, such that edges that cross at small angle have as different as posible.\n");
  fprintf(stderr, "Options are: \n");
  fprintf(stderr, " -accuracy=e      : accuracy with which to find the maximally different coloring for each node with regard to its neighbors. Default 0.01.\n");
  fprintf(stderr, " -angle=a         : if edge crossing is less than that angle a, then make the edge colors different. Default 15.\n");
  fprintf(stderr, " -random_seed=s   : random seed to use. s must be an integer. If s is negative, we do -s iterations with different seeds and pick the best.\n");
  fprintf(stderr, " -color_scheme=c  : palette used. The string c should be \"rgb\", \"gray\", \"lab\" (default); or\n");
  fprintf(stderr, "       a comma-separated list of RGB colors in hex (e.g., \"#ff0000,#aabbed,#eeffaa\"); or\n");
  fprintf(stderr, "       a string specifying a Brewer color scheme (e.g., \"accent7\"; see http://www.graphviz.org/content/color-names#brewer).\n");
  fprintf(stderr, " -lightness=l1,l2 : only applied for LAB color scheme: l1 must be integer >=0, l2 integer <=100, and l1 <=l2. By default we use 0,70\n");
  fprintf(stderr, " -share_endpoint  :  if this option is specified, edges that shares an end point are not considered in conflict if they are close to\n");
  fprintf(stderr, "       parallel but is on the opposite ends of the shared point (around 180 degree).\n");
  fprintf(stderr, " -v               : verbose\n");
  fprintf(stderr, " -o fname         :  write output to file fname (stdout)\n");
  exit(eval);
}

/* checkG:
 * Return non-zero if g has loops or multiedges.
 * Relies on multiedges occurring consecutively in edge list.
 */
static int
checkG (Agraph_t* g)
{
	Agedge_t* e;
	Agnode_t* n;
	Agnode_t* h;
	Agnode_t* prevh = NULL;

	for (n = agfstnode (g); n; n = agnxtnode (g, n)) {
		for (e = agfstout (g, n); e; e = agnxtout (g, e)) {
			if ((h = aghead(e)) == n) return 1;   // loop
			if (h == prevh) return 1;            // multiedge
			prevh = h;
		}
		prevh = NULL;  // reset
	}
	return 0;
}


static void init(int argc, char *argv[], real *angle, real *accuracy, char **infile, int *check_edges_with_same_endpoint, int *seed, char **color_scheme, char **lightness){

  unsigned int c;
  char* cmd = argv[0];
  outfile = NULL;

  Verbose = FALSE;
  *accuracy = 0.01;
  *angle = 15;/* 10 degree by default*/
  *infile = NULL;
  *check_edges_with_same_endpoint = 0;
  *seed = 123;
  *color_scheme = "lab";
  *lightness = NULL;

  while ((c = getopt(argc, argv, ":vc:a:s:r:l:o")) != -1) {
    switch (c) {
    case 's':
      *check_edges_with_same_endpoint = 1;
      break;
    case 'r':
      if (strncmp(optarg,"andom_seed=", 11) == 0){
	if (sscanf(optarg+11, "%d", seed) != 1){
	  fprintf(stderr,"-random_seed option must be a positive integer.\n");
	  usage(cmd, 1);
	}
      }
      break;
    case 'a':
      if (strncmp(optarg,"ccuracy=", 8) == 0){
	sscanf(optarg+8, "%lf", accuracy);
	if (*accuracy <= 0) {
	  fprintf(stderr,"-accuracy option must be a positive real number.\n");
	  usage(cmd, 1);
	}
      } else if (strncmp(optarg,"ngle=", 5) == 0){
	sscanf(optarg+5, "%lf", angle);
	if (*angle <= 0 || *angle >= 90) {
	  fprintf(stderr,"-angle option must be a positive real number between 0 to 90.\n");
	  usage(cmd, 1);
	}
      } else {
	fprintf(stderr,"-accuracy option must contain a positive real.\n");
	usage(cmd, 1);
      }
      break;
    case 'c':
      if (strncmp(optarg,"olor_scheme=", 12) == 0){
	if (knownColorScheme(optarg + 12))
          *color_scheme = optarg+12;
        else {
	  fprintf(stderr,"-color_scheme option must be a valid string\n");
	  usage(cmd, 1);
	}
      } else {
	usage(cmd, 1);
      }
      break;
    case 'l':{
      int l1 = 0, l2 = 70;
      if (strncmp(optarg,"ightness=", 9) == 0 && sscanf(optarg + 9, "%d,%d", &l1, &l2) == 2){
	if (l1 < 0 || l2 > 100 || l1 > l2){
	  fprintf(stderr,"invalid -lightness=%s option.\n", optarg + 9);
	  usage(cmd, 1);
	}
	*lightness = malloc(sizeof(char)*10);
	strcpy(*lightness, optarg + 9);
      } else {
	usage(cmd, 1);
      }
      break;
    }
    case 'v':
      Verbose = TRUE;
      break;
    case 'o':
      outfile = openFile(optarg, "w", CmdName);
      break;
    default:
      if (optopt == '?')
	usage(cmd, 0);
      else
	fprintf(stderr, "option -%c unrecognized - ignored\n",
		optopt);
      break;
    }
  }

  argv += optind;
  argc -= optind;
  
  if (argc)
    *infile = argv[0];

  if (argc > 0) Files = argv;
  if (!outfile) outfile = stdout;

}


static int clarify(Agraph_t* g, real angle, real accuracy, char *infile, int check_edges_with_same_endpoint, int seed, char *color_scheme, char *lightness){
  enum {buf_len = 10000};

  if (checkG(g)) {
    agerr (AGERR, "Graph %s (%s) contains loops or multiedges\n");
    return 1;
  }

  initDotIO(g);
  g = edge_distinct_coloring(color_scheme, lightness, g, angle, accuracy, check_edges_with_same_endpoint, seed);
  if (!g) return 1;

  agwrite (g, stdout);
  return 0;
}

static Agraph_t *gread(FILE * fp)
{
    return agread(fp, (Agdisc_t *) 0);
}

int main(int argc, char *argv[])
{
  char *infile;
  real accuracy;
  real angle;
  int check_edges_with_same_endpoint, seed;
  char *color_scheme = NULL;
  char *lightness = NULL;
  Agraph_t *g;
  Agraph_t *prev = NULL;
  ingraph_state ig;
  int rv = 0;

	init(argc, argv, &angle, &accuracy, &infile, &check_edges_with_same_endpoint, &seed, &color_scheme, &lightness);
	newIngraph(&ig, Files, gread);

	while ((g = nextGraph(&ig)) != 0) {
		if (prev)
		    agclose(prev);
		prev = g;
		fname = fileName(&ig);
		if (Verbose)
		    fprintf(stderr, "Process graph %s in file %s\n", agnameof(g),
			    fname);
		rv |= clarify(g, angle, accuracy, infile, check_edges_with_same_endpoint, seed, color_scheme, lightness);
	}

	return rv;
}
