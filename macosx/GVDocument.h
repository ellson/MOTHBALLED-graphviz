/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2008 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

@class GVExportViewController;
@class GVGraph;

@interface GVDocument : NSDocument
{
	GVExportViewController *_exporter;
	GVGraph *_graph;
}

@property(readonly) GVGraph *graph;

- (id)init;

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError;
- (BOOL)writeToURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError;

- (void)makeWindowControllers;

- (IBAction)exportDocument:(id)sender;
- (void)exporterDidEnd:(GVExportViewController *)exporter;

- (void)fileDidChange:(NSString *)path;
- (void)graphDidChange:(NSNotification *)notification;

- (void)dealloc;

@end
