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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32_DLL
#include "windows.h"
#endif
#include <stdio.h>
#include <QApplication>
#include "mainwindow.h"

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif


#ifdef WIN32_DLL
__declspec(dllimport) boolean MemTest;
/*gvc.lib cgraph.lib*/
#ifdef WITH_CGRAPH
    #pragma comment( lib, "cgraph.lib" )
#else
    #pragma comment( lib, "graph.lib" )
    #pragma comment( lib, "ingraphs.lib" )
    #pragma comment( lib, "gvc.lib" )
    #pragma comment( lib, "glib-2.0.lib" )
    #pragma comment( lib, "pango-1.0.lib" )
    #pragma comment( lib, "pangocairo-1.0.lib" )
    #pragma comment( lib, "cairo.lib" )
    #pragma comment( lib, "gobject-2.0.lib" )

#endif
    #pragma comment( lib, "gvc.lib" )
#endif

static char* cmd;

extern int Verbose;

static char *useString =
    "Usage: gvedit [-v?] <files>\n\
  -v - verbose\n\
  -? - print usage\n";

static void usage(int v)
{
    printf(useString);
    exit(v);
}

static char **parseArgs(int argc, char *argv[])
{
    int c;

    cmd = argv[0];
    while ((c = getopt(argc, argv, ":v?")) != -1) {
	switch (c) {
	case 'v':
	    Verbose = 1;
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else
		fprintf(stderr,
			"%s : option -%c unrecognized - ignored\n",
			cmd, optopt);
	    break;
	}
    }

    argv += optind;
    argc -= optind;

    if (argc)
	return argv;
    else
	return NULL;
}

int main(int argc, char *argv[])
{

    Q_INIT_RESOURCE(mdi);

    QApplication app(argc, argv);
    char** files = parseArgs (argc, argv);
    CMainWindow mainWin;
    if (files)
	while (*files) {
	    mainWin.addFile (QString(*files));
	    files++;
	}
    mainWin.show();
    return app.exec();
}
