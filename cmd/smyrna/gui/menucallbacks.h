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

#ifndef MENUCALLBACKS_H
#define MENUCALLBACKS_H

#include "gui.h"


//file
_BB void mNewSlot (GtkWidget *widget,gpointer user_data);
_BB void mOpenSlot (GtkWidget *widget,gpointer user_data);
_BB void mSaveSlot (GtkWidget *widget,gpointer user_data);
_BB void mSaveAsSlot (GtkWidget *widget,gpointer user_data);
_BB void mOptionsSlot (GtkWidget *widget,gpointer user_data);
_BB void mQuitSlot (GtkWidget *widget,gpointer user_data);

//edit
_BB void mCutSlot (GtkWidget *widget,gpointer user_data);
_BB void mCopySlot (GtkWidget *widget,gpointer user_data);
_BB void mPasteSlot (GtkWidget *widget,gpointer user_data);
_BB void mDeleteSlot (GtkWidget *widget,gpointer user_data);
_BB void mTopviewSettingsSlot (GtkWidget *widget,gpointer user_data);

//view
_BB void mShowToolBoxSlot(GtkWidget *widget,gpointer user_data);
_BB void mShowHostSelectionSlot(GtkWidget *widget,gpointer user_data);
//Graph
_BB void mNodeListSlot (GtkWidget *widget,gpointer user_data);
_BB void mNewNodeSlot (GtkWidget *widget,gpointer user_data);
_BB void mNewEdgeSlot (GtkWidget *widget,gpointer user_data);
_BB void mNewClusterSlot (GtkWidget *widget,gpointer user_data);
_BB void mGraphPropertiesSlot (GtkWidget *widget,gpointer user_data);
_BB void mClusterPropertiesSlot (GtkWidget *widget,gpointer user_data);
_BB void mNodePropertiesSlot (GtkWidget *widget,gpointer user_data);
_BB void mEdgePropertiesSlot (GtkWidget *widget,gpointer user_data);
_BB void mShowCodeSlot (GtkWidget *widget,gpointer user_data);
_BB void mDotSlot (GtkWidget *widget,gpointer user_data);
_BB void mNeatoSlot (GtkWidget *widget,gpointer user_data);
_BB void mTwopiSlot (GtkWidget *widget,gpointer user_data);
_BB void mCircoSlot (GtkWidget *widget,gpointer user_data);
_BB void mFdpSlot (GtkWidget *widget,gpointer user_data);

//select
_BB void  mSelectAllSlot(GtkWidget *widget,gpointer user_data);
_BB void  mUnselectAllSlot(GtkWidget *widget,gpointer user_data);
_BB void mSelectAllNodesSlot(GtkWidget *widget,gpointer user_data);
_BB void mSelectAllEdgesSlot(GtkWidget *widget,gpointer user_data);
_BB void mSelectAllClustersSlot(GtkWidget *widget,gpointer user_data);
_BB void mUnselectAllNodesSlot(GtkWidget *widget,gpointer user_data);
_BB void mUnselectAllEdgesSlot(GtkWidget *widget,gpointer user_data);
_BB void mUnselectAllClustersSlot(GtkWidget *widget,gpointer user_data);
_BB void mSingleSelectSlot(GtkWidget *widget,gpointer user_data);
_BB void mSelectAreaSlot(GtkWidget *widget,gpointer user_data);
_BB void mSelectAreaXSlot(GtkWidget *widget,gpointer user_data);

//help
_BB void mAbout(GtkWidget *widget,gpointer user_data);
_BB void mHelp(GtkWidget *widget,gpointer user_data);




#endif
