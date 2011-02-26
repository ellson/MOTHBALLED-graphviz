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


#ifdef WIN32_DLL
#include "windows.h"
#endif
#include <QApplication>

#include "mainwindow.h"

#ifdef WIN32_DLL
__declspec(dllimport) boolean MemTest;
/*gvc.lib cgraph.lib*/
#ifdef WITH_CGRAPH
    #pragma comment( lib, "cgraph.lib" )
#else
    #pragma comment( lib, "graph.lib" )
#endif
    #pragma comment( lib, "gvc.lib" )
#endif

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(mdi);

    QApplication app(argc, argv);
    CMainWindow mainWin;
    mainWin.show();
    return app.exec();
}
