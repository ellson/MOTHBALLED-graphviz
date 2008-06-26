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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
//windows.h for win machines
#if defined(_WIN32) && !defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <glade/glade.h>
#include "gui.h"
#include "viewport.h"
#include "menucallbacks.h"
#include "gltemplate.h"
#include "memory.h"
#ifdef ENABLE_NLS
#include "libintl.h"
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

#ifdef G_OS_WIN32
gchar *package_prefix;
gchar *package_data_dir;
#endif
gchar *package_locale_dir;
static char* smyrnaDir;
char* smyrnaGlade;

char*
smyrnaPath (char* suffix)
{
    char* buf;
    if (!smyrnaDir) return NULL;
    buf = N_NEW(strlen(smyrnaDir)+strlen(suffix)+2,char);
    sprintf (buf, "%s/%s", smyrnaDir, suffix);
    return buf;
}

static char *useString = "Usage: smyrn [-txv?] [-K<engine>] <file>\n\
  -n         - use TopView mode\n\
  -e         - use XDOT mode\n\
  -K<engine> - layout graph using <engine>\n\
  -v         - verbose\n\
  -?         - print usage\n";

static void usage(int v)
{
    printf(useString);
    exit(v);
}

static char*
parseArgs (int argc, char *argv[], ViewInfo* view)
{
    unsigned int c;

    while ((c = getopt(argc, argv, ":K:txv?")) != -1) {
	switch (c) {
	case 't':
	    view->dfltViewType = VT_TOPVIEW; 
	    break;
	case 'x':
	    view->dfltViewType = VT_XDOT; 
	    break;
	case 'K':
	    view->dfltEngine = s2layout (optarg);
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else
		fprintf(stderr, "smyrna: option -%c unrecognized - ignored\n",
			optopt);
	    break;
	}
    }

    if (optind < argc)
	 return argv[optind];
    else
	return NULL;
}

int main(int argc, char *argv[])
{
    GdkGLConfig *glconfig;
    char* initFileName;

    smyrnaDir = getenv ("SMYRNA_PATH");
#ifndef _WIN32
    if (!smyrnaDir)
	smyrnaDir = SMYRNA_PATH;
#endif

    load_attributes();

#ifdef G_OS_WIN32
    package_prefix =
	g_win32_get_package_installation_directory(NULL, NULL);
    package_data_dir = g_build_filename(package_prefix, "share", NULL);
    package_locale_dir =
	g_build_filename(package_prefix, "share", "locale", NULL);
#else
    if (smyrnaDir)
	package_locale_dir = g_build_filename(smyrnaDir, "locale", NULL);
    else
	package_locale_dir = g_build_filename(SMYRNA_PATH, "locale", NULL);
#endif	/* # */
#ifdef ENABLE_NLS
    bindtextdomain(GETTEXT_PACKAGE, package_locale_dir);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif
    view = NEW(ViewInfo);
    init_viewport(view);

    gtk_set_locale();
    gtk_init(&argc, &argv);
    initFileName = parseArgs (argc, argv, view);

#ifdef _WIN32
#define GTKTOPVIEW_ICONSDIR "C:\\Projects\\ATT\\GTK\\GTKTest2\\GUI\\images\\"
#endif
    if (!(smyrnaGlade)) {
#ifdef _WIN32
	smyrnaGlade = SMYRNA_GLADE;
#else
	smyrnaGlade = smyrnaPath ("smyrna.glade");
#endif
    }
    xml = glade_xml_new(smyrnaGlade, NULL, NULL);
    gladewidget = glade_xml_get_widget(xml, "frmMain");
    gtk_widget_show(gladewidget);
    g_signal_connect((gpointer) gladewidget, "destroy",
		     G_CALLBACK(mQuitSlot), NULL);
    glade_xml_signal_autoconnect(xml);
    gtk_gl_init(0, 0);
    /* Configure OpenGL framebuffer. */
    glconfig = configure_gl();
    gladewidget = glade_xml_get_widget(xml, "vbox2");
    create_window(glconfig, gladewidget);

    if (initFileName)
	add_graph_to_viewport_from_file(initFileName);
    gtk_main();


#ifdef G_OS_WIN32
    g_free(package_prefix);
    g_free(package_data_dir);
#endif
    g_free(package_locale_dir);
    clear_viewport(view);
    return 0;
}
