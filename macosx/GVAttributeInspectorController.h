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
#import <WebKit/WebKit.h>

@class GVDocument;

@interface GVAttributeInspectorController : NSWindowController {
	IBOutlet NSToolbar *componentToolbar;
	IBOutlet NSToolbarItem *graphToolbarItem;
	IBOutlet NSToolbarItem *nodeDefaultToolbarItem;
	IBOutlet NSToolbarItem *edgeDefaultToolbarItem;
	
	IBOutlet NSTableView *attributeTable;
	IBOutlet WebView *documentationWeb;
	
	NSDictionary *_allSchemas;
	NSMutableDictionary *_allAttributes;
	
	GVDocument *_inspectedDocument;
	BOOL _otherChangedGraph;
}

- (id)init;

- (void)awakeFromNib;

/* notifications */
- (IBAction)toolbarItemDidSelect:(id)sender;
- (void)graphWindowDidBecomeMain:(NSNotification *)notification;
- (void)graphDocumentDidChange:(NSNotification *)notification;
- (void)reloadAttributes;

/* toolbar delegate methods */
- (NSArray *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar;
 
/* table delegate methods */
- (NSCell *)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row;
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;

/* table data source methods */
- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)rowIndex;
- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)rowIndex;

- (void)dealloc;

@end
