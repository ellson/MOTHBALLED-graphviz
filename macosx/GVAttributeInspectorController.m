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

#import "GVAttributeInspectorController.h"
#import "GVAttributeSchema.h"
#import "GVGraph.h"
#import "GVWindowController.h"

@implementation GVAttributeInspectorController

- (id)init
{
	if (self = [super initWithWindowNibName: @"Attributes"]) {
		_allSchemas = nil;
		_allAttributes = [[NSMutableDictionary alloc] init];
		_inspectedGraph = nil;
		_otherChangedGraph = YES;
	}
	return self;
}

- (void)awakeFromNib
{
	/* set component toolbar */
	[_allSchemas release];
	_allSchemas = [[NSDictionary alloc] initWithObjectsAndKeys:
		[GVAttributeSchema attributeSchemasWithComponent:@"graph"], [graphToolbarItem itemIdentifier],
		[GVAttributeSchema attributeSchemasWithComponent:@"node"], [nodeDefaultToolbarItem itemIdentifier],
		[GVAttributeSchema attributeSchemasWithComponent:@"edge"], [edgeDefaultToolbarItem itemIdentifier],
		nil];
	[componentToolbar setSelectedItemIdentifier:[graphToolbarItem itemIdentifier]];
	[self toolbarItemDidSelect:nil];
		
	/* start observing whenever a window becomes main */
	[self graphWindowDidBecomeMain:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(graphWindowDidBecomeMain:) name:NSWindowDidBecomeMainNotification object:nil];
}

- (IBAction)toolbarItemDidSelect:(id)sender
{
	/* reload the table */
	[attributeTable reloadData];
}

- (void)graphWindowDidBecomeMain:(NSNotification *)notification
{
	NSWindow* mainWindow = notification ? [notification object] : [NSApp mainWindow];
	NSWindowController* mainWindowController = [mainWindow windowController];
	if ([mainWindowController respondsToSelector:@selector(graph)]) {
		GVGraph *newGraph = [(GVWindowController *)mainWindowController graph];
		
		if (_inspectedGraph != newGraph) {
			/* retain the inspected graph and start observing any changes from it */
			NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
			if (_inspectedGraph) {
				[defaultCenter removeObserver:self name:@"GVGraphDidChange" object:_inspectedGraph];
				[_inspectedGraph release];
			}
			if (_inspectedGraph = [newGraph retain]) {
				[_allAttributes setObject:newGraph.graphAttributes forKey:[graphToolbarItem itemIdentifier]];
				[_allAttributes setObject:newGraph.defaultNodeAttributes forKey:[nodeDefaultToolbarItem itemIdentifier]];
				[_allAttributes setObject:newGraph.defaultEdgeAttributes forKey:[edgeDefaultToolbarItem itemIdentifier]];
				[defaultCenter addObserver:self selector:@selector(graphDidChange:) name:@"GVGraphDidChange" object:newGraph];
			}
			else
				[_allAttributes removeAllObjects];
				
		
			/* update the UI */
			[[self window] setTitle:[NSString stringWithFormat:@"%@ Attributes", [mainWindow title]]];
			[attributeTable reloadData];
		}
	}
}

- (void)graphDidChange:(NSNotification *)notification
{
	/* if we didn't instigate the change, update the UI */
	if (_otherChangedGraph)
		[attributeTable reloadData];
}

- (NSArray *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar
{
	/* which toolbar items are selectable */
	return [NSArray arrayWithObjects:
		[graphToolbarItem itemIdentifier],
		[nodeDefaultToolbarItem itemIdentifier],
		[edgeDefaultToolbarItem itemIdentifier],
		nil];
}

- (NSCell *)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	if ([[tableColumn identifier] isEqualToString:@"value"]) {
		/* use the row's schema's cell */
		NSCell *cell = [[[_allSchemas objectForKey:[componentToolbar selectedItemIdentifier]] objectAtIndex:row] cell];
		[cell setEnabled:[_allAttributes count] > 0];
		return cell;
	}
	else
		/* use the default cell (usually a text field) for other columns */
		return nil;
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
	int selectedRow = [[aNotification object] selectedRow];
	NSString* documentation = selectedRow == -1 ? nil : [[[_allSchemas objectForKey:[componentToolbar selectedItemIdentifier]] objectAtIndex: selectedRow] documentation];
	[[documentationWeb mainFrame] loadHTMLString:documentation baseURL:[NSURL URLWithString:@"http://www.graphviz.org/"]];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [[_allSchemas objectForKey:[componentToolbar selectedItemIdentifier]] count];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)rowIndex
{
	NSString *selectedComponentIdentifier = [componentToolbar selectedItemIdentifier];
	NSString *attributeName = [[[_allSchemas objectForKey:selectedComponentIdentifier] objectAtIndex:rowIndex] name];
	if ([[tableColumn identifier] isEqualToString:@"key"])
		return attributeName;
	else if ([[tableColumn identifier] isEqualToString:@"value"])
		/* return the inspected graph's attribute value, if any */
		return [[_allAttributes objectForKey:selectedComponentIdentifier] valueForKey:attributeName];
	else
		return nil;
}

- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)rowIndex
{
	if ([[tableColumn identifier] isEqualToString:@"value"])
		{
			NSString *selectedComponentIdentifier = [componentToolbar selectedItemIdentifier];
			NSString *attributeName = [[[_allSchemas objectForKey:selectedComponentIdentifier] objectAtIndex:rowIndex] name];
			
			/* set or remove the key-value on the selected attributes */
			/* NOTE: to avoid needlessly reloading the table in graphDidChange:, we fence this change with _otherChangedGraph = NO */
			_otherChangedGraph = NO;
			@try {
				[[_allAttributes objectForKey:selectedComponentIdentifier] setValue:anObject forKey:attributeName];
			}
			@finally {
				_otherChangedGraph = YES;
			}
		}

}


- (void)dealloc
{
	[_allSchemas release];
	[_allAttributes release];
	
	[_inspectedGraph release];
	
	[super dealloc];
}

@end
