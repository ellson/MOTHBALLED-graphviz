#ifndef GVPRPIPE_H
#define GVPRPIPE_H
#define BUFSIZE 4096 
#ifdef WIN32
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include "cgraph.h"
extern Agraph_t* exec_gvpr(char* filename,Agraph_t* srcGraph);
#endif
#endif