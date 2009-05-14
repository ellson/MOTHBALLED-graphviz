/* $Id$Revision: */
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

#include "gvprpipe.h"
#include <stdio.h> 

#ifdef WIN32
#define BUFSIZE 4096 
#include <windows.h> 
#include <tchar.h>
#include <strsafe.h>
#endif

#include <const.h> 
#include <agxbuf.h> 

#ifdef WIN32

/* two pipes
 *  1 between smyrna and gvpr 
 *    SmyrnaToGvprIn : write end of pipe , active graph's fread uses
 *    SmyrnaToGvprROut: read end of pipe , created process' std in uses
 *  1 between gvpr and smyrna
 *    GvprToSmyrnaIn :write end of pipe, created process' stdout uses
 *    GvprToSmyrnaOut :read end of pipe, new graph's fread uses
 */

static HANDLE GvprToSmyrnaWr;
static HANDLE GvprToSmyrnaRd;
static HANDLE SmyrnaToGvprWr;
static HANDLE SmyrnaToGvprRd;
static SECURITY_ATTRIBUTES saAttr; 

/*cgraph wrappers*/
static int cgraph_write_wrapper(void *chan, char *str)
{
    DWORD dwWritten; 
    BOOL bSuccess = FALSE;
 
    bSuccess = WriteFile(SmyrnaToGvprWr, str, (DWORD)strlen(str), &dwWritten, NULL);
    if ( ! bSuccess ) return 0; 
    return dwWritten;
} 

static int cgraph_read_wrapper (void* ch,char* bf,int sz)
{
    DWORD dwRead;
    int ind=0;
    ReadFile( GvprToSmyrnaRd, bf, sz, &dwRead, NULL);
    printf("read returned value:%d\n",strlen(bf));
    printf("%s\n",bf);
    return dwRead;
}

/* ErrorExit:
 * Format a readable error message, display a message box, 
 * and exit from the application.
 */
static void
ErrorExit(PTSTR lpszFunction) 
{ 
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(1);
}

static Agraph_t*
ReadFromPipe() 
{ 
    BOOL bSuccess = FALSE;
    int totalbytes=0;
    int ind=0;
    Agiodisc_t a; 
    Agdisc_t disc;
    Agraph_t* g=0;

    if (!CloseHandle(SmyrnaToGvprWr))
	ErrorExit(TEXT("StdOutWr CloseHandle")); 

    a.afread=cgraph_read_wrapper; 
    a.flush=dummyflush;
    disc.io=&a;
    disc.id = &AgIdDisc;
    disc.mem = &AgMemDisc;
    g=agread(NULL,&disc);	
    return g; 
} 

static void
CreateChildProcess(TCHAR* szCmdline,HANDLE g_hChildStd_IN_Rd,HANDLE g_hChildStd_OUT_Wr)
{ 
   PROCESS_INFORMATION piProcInfo; 
   STARTUPINFO siStartInfo;
   BOOL bSuccess = FALSE; 
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 
   siStartInfo.hStdError = g_hChildStd_OUT_Wr;
   siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
   siStartInfo.hStdInput = g_hChildStd_IN_Rd;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
   bSuccess = CreateProcess(NULL, 
      szCmdline,     // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,             // creation flags 
      NULL,          // use parent's environment 
      NULL,          // use parent's current directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION 
   
	if ( ! bSuccess ) 
		ErrorExit(TEXT("CreateProcess"));
	else 
	{
      // Close handles to the child process and its primary thread.
	  // Some applications might keep these handles to monitor the status
	  // of the child process, for example. 
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}
}

static void init_security_attr(SECURITY_ATTRIBUTES* Attr)
{
   Attr->nLength = sizeof(SECURITY_ATTRIBUTES); 
   Attr->bInheritHandle = TRUE; 
   Attr->lpSecurityDescriptor = NULL; 
}

static void createpipes()
{
	if ( ! CreatePipe(&GvprToSmyrnaRd, &GvprToSmyrnaWr, &saAttr, 0) ) 
		ErrorExit(TEXT("StdoutRd CreatePipe")); 
	if ( ! SetHandleInformation(GvprToSmyrnaRd, HANDLE_FLAG_INHERIT, 0) )
		ErrorExit(TEXT("Stdout SetHandleInformation")); 
 
	if (! CreatePipe(&SmyrnaToGvprRd, &SmyrnaToGvprWr, &saAttr, 0)) 
		ErrorExit(TEXT("Stdin CreatePipe")); 
 
	if ( ! SetHandleInformation(SmyrnaToGvprWr, HANDLE_FLAG_INHERIT, 0) )
		ErrorExit(TEXT("Stdin SetHandleInformation")); 

}



static int dummyflush (void *chan)
{
    return 1;
}
#else
 
#endif

Agraph_t*
exec_gvpr(char* filename,Agraph_t* srcGraph)
{ 
    Agraph_t* G;
    unsigned char bf[SMALLBUF];
    agxbuf xbuf;
#ifdef WIN32
    Agiodisc_t* xio;	
    Agiodisc_t a; 

    init_security_attr(&saAttr);
    createpipes();
#else
    FILE* pp;
#endif

    agxbinit (&xbuf, SMALLBUF, bf);
    agxbput (&xbuf, "gvpr -c -f ");
    agxbput (&bf, filename);
   
#ifdef WIN32
    CreateChildProcess (agxbuse (&xbuf), SmyrnaToGvprRd, GvprToSmyrnaWr);

    xio = srcGraph->clos->disc.io;
    a.afread = srcGraph->clos->disc.io->afread; 
    a.putstr = cgraph_write_wrapper;
    a.flush = xio->flush;
    srcGraph->clos->disc.io = &a;
    agwrite(srcGraph,NULL);
    srcGraph->clos->disc.io = xio;
    G = ReadFromPipe() ;
#else
    pp = popen (agxbuse (&xbuf), "r+");
    agwrite(srcGraph, pp);
    G = agread(pp, NULL) ;
    pclose (pp);
#endif

    return G; 
} 
 
