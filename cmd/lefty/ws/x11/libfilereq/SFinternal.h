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

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Copyright 1989 Software Research Associates, Inc., Tokyo, Japan
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Software Research Associates
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * SOFTWARE RESEARCH ASSOCIATES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL SOFTWARE RESEARCH ASSOCIATES BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Erik M. van der Poel
 *         Software Research Associates, Inc., Tokyo, Japan
 *         erik@sra.co.jp
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SEL_FILE_CANCEL		-1
#define SEL_FILE_OK		0
#define SEL_FILE_NULL		1
#define SEL_FILE_TEXT		2

#define SF_DO_SCROLL		1
#define SF_DO_NOT_SCROLL	0

    typedef struct {
	int statDone;
	char *real;
	char *shown;
    } SFEntry;

    typedef struct {
	char *dir;
	char *path;
	SFEntry *entries;
	int nEntries;
	int vOrigin;
	int nChars;
	int hOrigin;
	int changed;
	int beginSelection;
	int endSelection;
	time_t mtime;
    } SFDir;

    extern int SFstatus;

    extern char SFcurrentPath[], SFstartDir[], SFcurrentDir[];

    extern Widget
	selFile,
	selFileCancel,
	selFileField,
	selFileForm,
	selFileHScroll,
	selFileHScrolls[],
	selFileLists[], selFileOK, selFilePrompt, selFileVScrolls[];

    extern Display *SFdisplay;

    extern int SFcharWidth, SFcharHeight, SFcharAscent;

    extern SFDir *SFdirs;

    extern int SFdirEnd, SFdirPtr;

    extern Pixel SFfore, SFback;

    extern Atom SFwmDeleteWindow;

    extern XSegment SFsegs[], SFcompletionSegs[];

    extern XawTextPosition SFtextPos;

    extern void SFenterList(Widget w, int n, XEnterWindowEvent *event);
    extern void SFleaveList(Widget w, int n, XEvent *event);
    extern void SFmotionList(Widget w, int n, XMotionEvent *event);
    extern void SFbuttonPressList(Widget w, int n, XButtonPressedEvent *event);
    extern void SFbuttonReleaseList(Widget w, int n, XButtonReleasedEvent *event);

    extern void SFvSliderMovedCallback(Widget w, int n, int fnew);
    extern void SFvFloatSliderMovedCallback(Widget w, int n, float *new);
    extern void SFhSliderMovedCallback(Widget w, int n, float *new);
    extern void SFpathSliderMovedCallback(Widget w, XtPointer client_data, float *new);
    extern void SFvAreaSelectedCallback(Widget w, int n, int pnew);
    extern void SFhAreaSelectedCallback(Widget w, int n, int pnew);
    extern void SFpathAreaSelectedCallback(Widget w, XtPointer client_data, int pnew);

    extern int SFupperX, SFlowerY, SFupperY;

    extern int SFtextX, SFtextYoffset;

    extern int SFentryWidth, SFentryHeight;

    extern int SFlineToTextH, SFlineToTextV;

    extern int SFbesideText, SFaboveAndBelowText;

    extern int SFcharsPerEntry;

    extern int SFlistSize;

    extern int SFcurrentInvert[];

    extern int SFworkProcAdded;
    extern XtWorkProcId SFworkProcId;

    extern Boolean SFworkProc(void);

    extern XtAppContext SFapp;

    extern int SFpathScrollWidth, SFvScrollHeight, SFhScrollWidth;

    extern char SFtextBuffer[];

    extern int SFbuttonPressed;

    extern int SFcompareEntries(const void *vp, const void *vq);

    extern void SFdirModTimer(XtPointer cl, XtIntervalId *id);

    extern char SFstatChar(struct stat *statBuf);

    extern XtIntervalId SFdirModTimerId;

    extern int (*SFfunc) (char *, char **, struct stat *);

#ifdef __cplusplus
}
#endif
