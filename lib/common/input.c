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

#include	"render.h"
#include	"htmltable.h"
#include	<ctype.h>

#if !defined(DISABLE_CODEGENS) && !defined(HAVE_GD_FREETYPE)
codegen_t *Output_codegen;
#endif

#if 0
static char *CONFIG = "cairo {renderer {svg svgz x11 png ps}}";
#else
static char *CONFIG = "";
#endif

char *Gvfilepath;

static char *usageFmt =
    "Usage: %s [-Vv?] [-(GNE)name=val] [-(Tlso)<val>] <dot files>\n";

static char *genericItems = "\n\
 -V          - Print version and exit\n\
 -v          - Enable verbose mode \n\
 -Gname=val  - Set graph attribute 'name' to 'val'\n\
 -Nname=val  - Set node attribute 'name' to 'val'\n\
 -Ename=val  - Set edge attribute 'name' to 'val'\n\
 -Tv         - Set output format to 'v'\n\
 -lv         - Use external library 'v'\n\
 -ofile      - Write output to 'file'\n\
 -q[l]       - Set level of message suppression (=1)\n\
 -s[v]       - Scale input by 'v' (=72)\n\
 -y          - Invert y coordinate in output\n";

static char *neatoFlags =
    "(additional options for neato)    [-x] [-n<v>]\n";
static char *neatoItems = "\n\
 -n[v]       - No layout mode 'v' (=1)\n\
 -x          - Reduce graph\n";

static char *fdpFlags =
    "(additional options for fdp)      [-L(gN)] [-L(nMUvSKsT)<val>]\n";
static char *fdpItems = "\n\
 -Lg         - Don't use grid\n\
 -LO         - Use old attractive force\n\
 -Ln<i>      - Set number of iterations to i\n\
 -LM<i>      - Set max. number of iterations to i\n\
 -LU<i>      - Set unscaled factor to i\n\
 -LS<i>      - Set seed to i\n\
 -LC<v>      - Set overlap expansion factor to v\n\
 -LK<v>      - Set desired edge length to v\n\
 -Ls<v>      - Set PS output scale to v\n\
 -Lt<i>      - Set number of tries to remove overlaps to i\n\
 -LT[*]<v>   - Set temperature (temperature factor) to v\n";

static char *memtestFlags = "(additional options for memtest)  [-m]\n";
static char *memtestItems = "\n\
 -m          - Memory test (Observe no growth with top. Kill when done.)\n";

void dotneato_usage(int exval)
{
    FILE *outs;

    if (exval > 0)
	outs = stderr;
    else
	outs = stdout;

    fprintf(outs, usageFmt, CmdName);
    fputs(neatoFlags, outs);
    fputs(fdpFlags, outs);
    fputs(memtestFlags, outs);
    fputs(genericItems, outs);
    fputs(neatoItems, outs);
    fputs(fdpItems, outs);
    fputs(memtestItems, outs);

    if (exval >= 0)
	exit(exval);
}

void setCmdName(char *s)
{
    char *n = strrchr(s, '/');

    if (n)
	CmdName = n + 1;
    else
	CmdName = s;
}

/* getFlagOpt:
 * Look for flag parameter. idx is index of current argument.
 * We assume argv[*idx] has the form "-x..." If there are characters 
 * after the x, return
 * these, else if there are more arguments, return the next one,
 * else return NULL.
 */
static char *getFlagOpt(int argc, char **argv, int *idx)
{
    int i = *idx;
    char *arg = argv[i];

    if (arg[2])
	return arg + 2;
    if (i < argc - 1) {
	i++;
	arg = argv[i];
	if (*arg && (*arg != '-')) {
	    *idx = i;
	    return arg;
	}
    }
    return 0;
}

void dotneato_initialize(GVC_t * gvc, int argc, char **argv)
{
    char *rest, c, *val;
    int i, v, nfiles;

    /* establish if we are running in a CGI environment */
    HTTPServerEnVar = getenv("SERVER_NAME");

    /* establish Gvfilepath, if any */
    Gvfilepath = getenv("GV_FILE_PATH");

    aginit();
    nfiles = 0;
    for (i = 1; i < argc; i++)
	if (argv[i] && argv[i][0] != '-')
	    nfiles++;
    Files = N_NEW(nfiles + 1, char *);
    nfiles = 0;
    if (!CmdName)
	setCmdName(argv[0]);
    for (i = 1; i < argc; i++) {
	if (argv[i] && argv[i][0] == '-') {
	    rest = &(argv[i][2]);
	    switch (c = argv[i][1]) {
	    case 'G':
		if (*rest)
		    global_def(rest, agraphattr);
		else {
		    fprintf(stderr, "Missing argument for -G flag\n");
		    dotneato_usage(1);
		}
		break;
	    case 'N':
		if (*rest)
		    global_def(rest, agnodeattr);
		else {
		    fprintf(stderr, "Missing argument for -N flag\n");
		    dotneato_usage(1);
		}
		break;
	    case 'E':
		if (*rest)
		    global_def(rest, agedgeattr);
		else {
		    fprintf(stderr, "Missing argument for -E flag\n");
		    dotneato_usage(1);
		}
		break;
	    case 'T':
		val = getFlagOpt(argc, argv, &i);
		if (!val) {
		    fprintf(stderr, "Missing argument for -T flag\n");
		    dotneato_usage(1);
		}
		gvrender_output_langname_job(gvc, val);
		break;
	    case 'V':
		fprintf(stderr, "%s version %s (%s)\n",
			gvc->info[0], gvc->info[1], gvc->info[2]);
		exit(0);
		break;
	    case 'l':
		val = getFlagOpt(argc, argv, &i);
		if (!val) {
		    fprintf(stderr, "Missing argument for -l flag\n");
		    dotneato_usage(1);
		}
		use_library(val);
		break;
	    case 'n':
		if (*rest) {
		    Nop = atoi(rest);
		    if (Nop <= 0) {
			fprintf(stderr,
				"Invalid parameter \"%s\" for -n flag\n",
				rest);
			dotneato_usage(1);
		    }
		} else
		    Nop = 1;
		break;
	    case 'o':
		val = getFlagOpt(argc, argv, &i);
		gvrender_output_filename_job(gvc, val);
		break;
	    case 'q':
		if (*rest) {
		    v = atoi(rest);
		    if (v <= 0) {
			fprintf(stderr,
				"Invalid parameter \"%s\" for -q flag - ignored\n",
				rest);
		    } else if (v == 1)
			agseterr(AGERR);
		    else
			agseterr(AGMAX);
		} else
		    agseterr(AGERR);
		break;
	    case 's':
		if (*rest) {
		    PSinputscale = atof(rest);
		    if (PSinputscale <= 0) {
			fprintf(stderr,
				"Invalid parameter \"%s\" for -s flag\n",
				rest);
			dotneato_usage(1);
		    }
		} else
		    PSinputscale = POINTS_PER_INCH;
		break;
	    case 'v':
		Verbose = TRUE;
		if (isdigit(*(unsigned char *) rest))
		    Verbose = atoi(rest);
		break;
	    case 'x':
		Reduce = TRUE;
		break;
	    case 'y':
		y_invert = TRUE;
		break;
	    case '?':
		dotneato_usage(0);
		break;
	    default:
		fprintf(stderr, "%s: option -%c unrecognized\n\n", CmdName,
			c);
		dotneato_usage(1);
	    }
	} else if (argv[i])
	    Files[nfiles++] = argv[i];
    }

    /* if no -Txxx, then set default format */
    if (!gvc->jobs || !gvc->jobs->output_langname) {
	gvrender_output_langname_job(gvc, "dot");
    }

    config_codegen_builtins(gvc);
    gvplugin_builtins(gvc);
    gvconfig(gvc, CONFIG);

#if !defined(DISABLE_CODEGENS) && !defined(HAVE_GD_FREETYPE)
    Output_codegen = gvc->codegen;
#endif

    /* set persistent attributes here (if not already set from command line options) */
    if (!(agfindattr(agprotograph()->proto->n, "label")))
	agnodeattr(NULL, "label", NODENAME_ESC);
}

void global_def(char *dcl,
		attrsym_t * ((*dclfun) (Agraph_t *, char *, char *)))
{
    char *p, *rhs = "true";
    attrsym_t *sym;
    if ((p = strchr(dcl, '='))) {
	*p++ = '\0';
	rhs = p;
    }
    sym = dclfun(NULL, dcl, rhs);
    sym->fixed = 1;
}

/* getdoubles2pt:
 * converts a graph attribute to a point.
 * Returns true if the attribute ends in '!'.
 */
static boolean getdoubles2pt(graph_t * g, char *name, point * result)
{
    char *p;
    int i;
    double xf, yf;
    char c = '\0';
    boolean rv = FALSE;

    if ((p = agget(g, name))) {
	i = sscanf(p, "%lf,%lf%c", &xf, &yf, &c);
	if ((i > 1) && (xf > 0) && (yf > 0)) {
	    result->x = POINTS(xf);
	    result->y = POINTS(yf);
	    if (c == '!')
		rv = TRUE;
	}
    }
    return rv;
}

void getdouble(graph_t * g, char *name, double *result)
{
    char *p;
    double f;

    if ((p = agget(g, name))) {
	if (sscanf(p, "%lf", &f) >= 1)
	    *result = f;
    }
}

FILE *next_input_file(void)
{
    static int ctr = 0;
    FILE *rv = NULL;

    if (Files[0] == NULL) {
	if (ctr++ == 0)
	    rv = stdin;
    } else {
	rv = NULL;
	while (Files[ctr]) {
	    if ((rv = fopen(Files[ctr++], "r")))
		break;
	    else {
		agerr(AGERR, "%s: can't open %s\n", CmdName,
		      Files[ctr - 1]);
		graphviz_errors++;
	    }
	}
    }
    if (rv)
	agsetfile(Files[0] ? Files[ctr - 1] : "<stdin>");
    return rv;
}

graph_t *next_input_graph(void)
{
    graph_t *g;
    static FILE *fp;

    if (fp == NULL)
	fp = next_input_file();
    g = NULL;

    while (fp != NULL) {
	if ((g = agread(fp)))
	    break;
	fp = next_input_file();
    }
    return g;
}

void graph_init(graph_t * g)
{
#if !defined(DISABLE_CODEGENS) && !defined(HAVE_GD_FREETYPE)
    initDPI(g);
#endif
    /* initialize the graph */
    init_ugraph(g);

    /* initialize nodes */
    N_height = agfindattr(g->proto->n, "height");
    N_width = agfindattr(g->proto->n, "width");
    N_shape = agfindattr(g->proto->n, "shape");
    N_color = agfindattr(g->proto->n, "color");
    N_fillcolor = agfindattr(g->proto->n, "fillcolor");
    N_style = agfindattr(g->proto->n, "style");
    N_fontsize = agfindattr(g->proto->n, "fontsize");
    N_fontname = agfindattr(g->proto->n, "fontname");
    N_fontcolor = agfindattr(g->proto->n, "fontcolor");
    N_label = agfindattr(g->proto->n, "label");
    N_showboxes = agfindattr(g->proto->n, "showboxes");
    /* attribs for polygon shapes */
    N_sides = agfindattr(g->proto->n, "sides");
    N_peripheries = agfindattr(g->proto->n, "peripheries");
    N_skew = agfindattr(g->proto->n, "skew");
    N_orientation = agfindattr(g->proto->n, "orientation");
    N_distortion = agfindattr(g->proto->n, "distortion");
    N_fixed = agfindattr(g->proto->n, "fixedsize");
    N_nojustify = agfindattr(g->proto->n, "nojustify");
    N_layer = agfindattr(g->proto->n, "layer");
    N_group = agfindattr(g->proto->n, "group");
    N_comment = agfindattr(g->proto->n, "comment");
    N_vertices = agfindattr(g->proto->n, "vertices");
    N_z = agfindattr(g->proto->n, "z");

    /* initialize edges */
    E_weight = agfindattr(g->proto->e, "weight");
    E_color = agfindattr(g->proto->e, "color");
    E_fontsize = agfindattr(g->proto->e, "fontsize");
    E_fontname = agfindattr(g->proto->e, "fontname");
    E_fontcolor = agfindattr(g->proto->e, "fontcolor");
    E_label = agfindattr(g->proto->e, "label");
    E_label_float = agfindattr(g->proto->e, "labelfloat");
    /* vladimir */
    E_dir = agfindattr(g->proto->e, "dir");
    E_arrowhead = agfindattr(g->proto->e, "arrowhead");
    E_arrowtail = agfindattr(g->proto->e, "arrowtail");
    E_headlabel = agfindattr(g->proto->e, "headlabel");
    E_taillabel = agfindattr(g->proto->e, "taillabel");
    E_labelfontsize = agfindattr(g->proto->e, "labelfontsize");
    E_labelfontname = agfindattr(g->proto->e, "labelfontname");
    E_labelfontcolor = agfindattr(g->proto->e, "labelfontcolor");
    E_labeldistance = agfindattr(g->proto->e, "labeldistance");
    E_labelangle = agfindattr(g->proto->e, "labelangle");
    /* end vladimir */
    E_minlen = agfindattr(g->proto->e, "minlen");
    E_showboxes = agfindattr(g->proto->e, "showboxes");
    E_style = agfindattr(g->proto->e, "style");
    E_decorate = agfindattr(g->proto->e, "decorate");
    E_arrowsz = agfindattr(g->proto->e, "arrowsize");
    E_constr = agfindattr(g->proto->e, "constraint");
    E_layer = agfindattr(g->proto->e, "layer");
    E_comment = agfindattr(g->proto->e, "comment");
    E_tailclip = agfindattr(g->proto->e, "tailclip");
    E_headclip = agfindattr(g->proto->e, "headclip");
}

/* setRatio:
 * Checks "ratio" attribute, if any, and sets enum type.
 */
static void setRatio(graph_t * g)
{
    char *p, c;
    double ratio;

    if ((p = agget(g, "ratio")) && ((c = p[0]))) {
	switch (c) {
	case 'a':
	    if (streq(p, "auto"))
		GD_drawing(g)->ratio_kind = R_AUTO;
	    break;
	case 'c':
	    if (streq(p, "compress"))
		GD_drawing(g)->ratio_kind = R_COMPRESS;
	    break;
	case 'e':
	    if (streq(p, "expand"))
		GD_drawing(g)->ratio_kind = R_EXPAND;
	    break;
	case 'f':
	    if (streq(p, "fill"))
		GD_drawing(g)->ratio_kind = R_FILL;
	    break;
	default:
	    ratio = atof(p);
	    if (ratio > 0.0) {
		GD_drawing(g)->ratio_kind = R_VALUE;
		GD_drawing(g)->ratio = ratio;
	    }
	    break;
	}
    }
}

void init_ugraph(graph_t * g)
{
    char *p;
    double xf;
    static char *rankname[] = { "local", "global", "none", NULL };
    static int rankcode[] = { LOCAL, GLOBAL, NOCLUST, LOCAL };

    GD_drawing(g) = NEW(layout_t);

    /* set this up fairly early in case any string sizes are needed */
    if ((p = agget(g, "fontpath")) || (p = getenv("DOTFONTPATH"))) {
	/* overide GDFONTPATH in local environment if dot
	 * wants its own */
#ifdef HAVE_SETENV
	setenv("GDFONTPATH", p, 1);
#else
	static char *buf = 0;

	buf = grealloc(buf, strlen("GDFONTPATH=") + strlen(p) + 1);
	strcpy(buf, "GDFONTPATH=");
	strcat(buf, p);
	putenv(buf);
#endif
    }

    GD_drawing(g)->quantum =
	late_double(g, agfindattr(g, "quantum"), 0.0, 0.0);

    /* setting rankdir=LR is only defined in dot,
     * but having it set causes shape code and others to use it. 
     * The result is confused output, so we turn it off unless requested.
     */
    GD_rankdir(g) = RANKDIR_TB;
    if (UseRankdir && (p = agget(g, "rankdir"))) {
	if (streq(p, "LR"))
	    GD_rankdir(g) = RANKDIR_LR;
	else if (streq(p, "BT"))
	    GD_rankdir(g) = RANKDIR_BT;
	else if (streq(p, "RL"))
	    GD_rankdir(g) = RANKDIR_RL;
    }
    do_graph_label(g);
    xf = late_double(g, agfindattr(g, "nodesep"), DEFAULT_NODESEP,
		     MIN_NODESEP);
    GD_nodesep(g) = POINTS(xf);

    p = late_string(g, agfindattr(g, "ranksep"), NULL);
    if (p) {
	if (sscanf(p, "%lf", &xf) == 0)
	    xf = DEFAULT_RANKSEP;
	else {
	    if (xf < MIN_RANKSEP)
		xf = MIN_RANKSEP;
	}
	if (strstr(p, "equally"))
	    GD_exact_ranksep(g) = TRUE;
    } else
	xf = DEFAULT_RANKSEP;
    GD_ranksep(g) = POINTS(xf);

    GD_showboxes(g) = late_int(g, agfindattr(g, "showboxes"), 0, 0);

    setRatio(g);
    GD_drawing(g)->filled =
	getdoubles2pt(g, "size", &(GD_drawing(g)->size));
    getdoubles2pt(g, "page", &(GD_drawing(g)->page));
    getdouble(g, "nodesep", &Nodesep);
    getdouble(g, "nodefactor", &Nodefactor);

    GD_drawing(g)->centered = mapbool(agget(g, "center"));
    if ((p = agget(g, "rotate")))
	GD_drawing(g)->landscape = (atoi(p) == 90);
    else {			/* today we learned the importance of backward compatibilty */
	if ((p = agget(g, "orientation")))
	    GD_drawing(g)->landscape = ((p[0] == 'l') || (p[0] == 'L'));
    }

    p = agget(g, "clusterrank");
    CL_type = maptoken(p, rankname, rankcode);
    p = agget(g, "concentrate");
    Concentrate = mapbool(p);

    GD_drawing(g)->dpi = 0.0;
    if (((p = agget(g, "dpi")) && p[0])
	|| ((p = agget(g, "resolution")) && p[0]))
	GD_drawing(g)->dpi = atof(p);

    Nodesep = 1.0;
    Nodefactor = 1.0;
    Initial_dist = MYHUGE;
}

void free_ugraph(graph_t * g)
{
    free(GD_drawing(g));
    GD_drawing(g) = NULL;
}

/* do_graph_label:
 * Set characteristics of graph label if it exists.
 * 
 */
void do_graph_label(graph_t * sg)
{
    char *p, *pos, *just;
    int pos_ix;

    /* it would be nice to allow multiple graph labels in the future */
    if ((p = agget(sg, "label"))) {
	char pos_flag;
	int html = aghtmlstr(p);
	point dpt;
	pointf dimen;

	GD_has_labels(sg->root) |= GRAPH_LABEL;
	GD_label(sg) = make_label(html, strdup_and_subst_graph(p, sg),
				  late_double(sg,
					      agfindattr(sg, "fontsize"),
					      DEFAULT_FONTSIZE,
					      MIN_FONTSIZE),
				  late_nnstring(sg,
						agfindattr(sg, "fontname"),
						DEFAULT_FONTNAME),
				  late_nnstring(sg,
						agfindattr(sg,
							   "fontcolor"),
						DEFAULT_COLOR), sg);
	if (html) {
	    if (make_html_label(GD_label(sg), sg))
		agerr(AGPREV, "in label of graph %s\n", sg->name);
	}

	/* set label position */
	pos = agget(sg, "labelloc");
	if (sg != sg->root) {
	    if (pos && (pos[0] == 'b'))
		pos_flag = LABEL_AT_BOTTOM;
	    else
		pos_flag = LABEL_AT_TOP;
	} else {
	    if (pos && (pos[0] == 't'))
		pos_flag = LABEL_AT_TOP;
	    else
		pos_flag = LABEL_AT_BOTTOM;
	}
	just = agget(sg, "labeljust");
	if (just) {
	    if (just[0] == 'l')
		pos_flag |= LABEL_AT_LEFT;
	    else if (just[0] == 'r')
		pos_flag |= LABEL_AT_RIGHT;
	}
	GD_label_pos(sg) = pos_flag;

	if (sg == sg->root)
	    return;

	/* Set border information for cluster labels to allow space
	 */
	dimen = GD_label(sg)->dimen;
	PAD(dimen);
	PF2P(dimen, dpt);
	if (!GD_flip(sg->root)) {
	    if (GD_label_pos(sg) & LABEL_AT_TOP)
		pos_ix = TOP_IX;
	    else
		pos_ix = BOTTOM_IX;
	    GD_border(sg)[pos_ix] = dpt;
	} else {
	    /* when rotated, the labels will be restored to TOP or BOTTOM  */
	    if (GD_label_pos(sg) & LABEL_AT_TOP)
		pos_ix = RIGHT_IX;
	    else
		pos_ix = LEFT_IX;
	    GD_border(sg)[pos_ix].x = dpt.y;
	    GD_border(sg)[pos_ix].y = dpt.x;
	}
    }
}

void dotneato_terminate(GVC_t * gvc)
{
    dotneato_eof(gvc);
    exit(graphviz_errors + agerrors());
}
