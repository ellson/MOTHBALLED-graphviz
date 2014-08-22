//---------------------------------------------------------------------------
// libdot - A standalone x86 or x64 DLL with no dependencies and a simple API 
// to interface with the DOT layout algorythm. No support for rendering a graph
// is provided.
//
// To build you will need to have all the expected auto genned files (ast_common.h, 
// builddate.h, config.h, types.h and so on) present.
//---------------------------------------------------------------------------
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <cgraph.h>
#include <gvc.h>
#include <ctype.h>

#define LIBDOT_DLL_LINK
#include "libdot.h"
//---------------------------------------------------------------------------
#pragma comment( lib, "cdt.lib" )
#pragma comment( lib, "cgraph.lib" )
#pragma comment( lib, "common.lib" )
#pragma comment( lib, "dotgen.lib" )
#pragma comment( lib, "gvc.lib" )
#pragma comment( lib, "gvplugin_core.lib" )
#pragma comment( lib, "gvplugin_dot_layout.lib" )
#pragma comment( lib, "label.lib" )
#pragma comment( lib, "pack.lib" )
#pragma comment( lib, "pathplan.lib" )
#pragma comment( lib, "xdot.lib" )

extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
//---------------------------------------------------------------------------
#include "htmltable.h"

htmllabel_t *parseHTML(char* txt, int* warn, htmlenv_t *env)
{
	return NULL;
}
//---------------------------------------------------------------------------
typedef struct _LIBDOT_STATE
{
	GVC_t * gvc;

	Agraph_t * graph;

	Agdesc_t desc;

} LIBDOT_STATE;
//---------------------------------------------------------------------------
void * libdot_Init( void )
{
	LIBDOT_STATE * state;
	
	state = (LIBDOT_STATE *)malloc( sizeof(LIBDOT_STATE) );

	memset( (void *)state, 0, sizeof(LIBDOT_STATE) );

	state->gvc = gvContext();

	if( !state->gvc )
	{
		free( state );
		return NULL;
	}

	gvAddLibrary( state->gvc, &gvplugin_dot_layout_LTX_library );

	state->graph = NULL;

	state->desc.directed = TRUE;

	return (void *)state;
}
//---------------------------------------------------------------------------
BOOL libdot_GraphCreate( void * state_ptr )
{
	LIBDOT_STATE * state;
	
	if( !state_ptr )
		return FALSE;

	state = (LIBDOT_STATE *)state_ptr;

	if( state->graph )
	{
		agclose( state->graph );

		state->graph = NULL;
    }

	state->graph = agopen( "g", state->desc, 0 );
	if( !state->graph )
		return FALSE;

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL libdot_GraphDestroy( void * _state_ptr )
{
	LIBDOT_STATE * state;

	if( !_state_ptr )
		return FALSE;

	state = (LIBDOT_STATE *)_state_ptr;
	if( !state->graph )
		return FALSE;

	agclose( state->graph );

	state->graph = NULL;

	return TRUE;
}
//---------------------------------------------------------------------------
void libdot_Cleanup( void * _state_ptr )
{
	LIBDOT_STATE * state;
	
	if( !_state_ptr )
		return;

	state = (LIBDOT_STATE *)_state_ptr;

	if( state->graph )
	{
		if( state->gvc )
			gvFreeLayout( state->gvc, state->graph );

		agclose( state->graph );

		state->graph = NULL;
	}

	if( state->gvc )
		gvFreeContext( state->gvc );

	free( state );
}
//---------------------------------------------------------------------------
BOOL libdot_GraphLayout( void * _state_ptr )
{
	LIBDOT_STATE * state;
	
	if( !_state_ptr )
		return FALSE;

	state = (LIBDOT_STATE *)_state_ptr;
	if( !state->gvc || !state->graph )
		return FALSE;

	if( gvLayout( state->gvc, state->graph, "dot" ) != 0 )
		return FALSE;

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL libdot_GraphPosition( void * _state_ptr, long * _width, long * _height )
{
	LIBDOT_STATE * state;
	
	if( !_state_ptr )
		return FALSE;

	state = (LIBDOT_STATE *)_state_ptr;
	if( !state->graph )
		return FALSE;

	if( _width )
		*_width = ROUND( GD_bb( state->graph ).UR.x );

	if( _height )
		*_height = ROUND( GD_bb( state->graph ).UR.y );

	return TRUE;
}
//---------------------------------------------------------------------------
void * libdot_NodeCreate( void * _state_ptr, long _width, long _height )
{
	LIBDOT_STATE * state;
	Agnode_t * node = NULL;
	char buffer[32];

	do
	{
		if( !_state_ptr )
			break;

		state = (LIBDOT_STATE *)_state_ptr;
		if( !state->graph )
			break;

		node = agnode( state->graph, NULL, TRUE );
		if( !node )
			break;

		_snprintf( buffer, sizeof(buffer), "%f", PS2INCH( _width ) );

		agsafeset( node, "width", buffer, "" );

		_snprintf( buffer, sizeof(buffer), "%f", PS2INCH( _height ) );

		agsafeset( node, "height", buffer, "" );

		// XXX: shame we cant do this directly...
		//ND_width( node )  = PS2INCH( _width );
		//ND_height( node ) = PS2INCH( _height );

	} while( 0 );

	return node;
}
//---------------------------------------------------------------------------
void libdot_NodeSetAttribute( void * _state_ptr, void * _node, char * _name, char * _value )
{
	if( !_state_ptr || !_node || !_name || !_value )
		return;

	agsafeset( _node, _name, _value, "" );
}
//---------------------------------------------------------------------------
void libdot_EdgeSetAttribute( void * _state_ptr, void * _edge, char * _name, char * _value )
{
	if( !_state_ptr || !_edge || !_name || !_value )
		return;

	agsafeset( _edge, _name, _value, "" );
}
//---------------------------------------------------------------------------
void libdot_GraphSetAttribute( void * _state_ptr, char * _name, char * _value )
{
	LIBDOT_STATE * state;
	
	if( !_state_ptr || !_name || !_value )
		return;
	
	state = (LIBDOT_STATE *)_state_ptr;
	if( !state->graph )
		return;

	agsafeset( state->graph, _name, _value, "" );
}
//---------------------------------------------------------------------------
void * libdot_EdgeCreate( void * _state_ptr, void * _origin_node, void * _dest_node )
{
	LIBDOT_STATE * state;
	Agedge_t * edge = NULL;

	do
	{
		if( !_state_ptr || !_origin_node || !_dest_node )
			break;

		state = (LIBDOT_STATE *)_state_ptr;
		if( !state->graph )
			break;

		edge = agedge( state->graph, (Agnode_t *)_origin_node, (Agnode_t *)_dest_node, NULL, TRUE );
		if( !edge )
			break;
		
	} while( 0 );

	return edge;
}
//---------------------------------------------------------------------------
BOOL libdot_NodePosition( void * _state_ptr, void * _node, long * _x, long * _y, long * _width, long * _height )
{
	LIBDOT_STATE * state;
	double w;
	double h;

	if( !_state_ptr || !_node )
		return FALSE;

	state = (LIBDOT_STATE *)_state_ptr;
	if( !state->graph )
		return FALSE;

	w = INCH2PS( ND_width( (Agnode_t *)_node ) );
	h = INCH2PS( ND_height( (Agnode_t *)_node ) );

	// Note: the nodex x,y coords are for the center of the rectangle, so turn these into top,left coords.
	// http://www.mupuf.org/blog/2010/07/08/how_to_use_graphviz_to_draw_graphs_in_a_qt_graphics_scene/

	if( _x )
		*_x = ROUND( ((ND_coord( (Agnode_t *)_node ).x - (w/2)) * (DEFAULT_DPI/POINTS_PER_INCH)) );

	if( _y )
		*_y = ROUND( ((ND_coord( (Agnode_t *)_node ).y - (h/2)) * (DEFAULT_DPI/POINTS_PER_INCH)) );

	if( _width )
		*_width  = ROUND( w );

	if( _height )
		*_height = ROUND( h );

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL libdot_EdgePosition( void * _state_ptr, void * _edge, long * _count, long ** _points )
{
	LIBDOT_STATE * state;
	splines * spl;
	int i;
	long count;
	long * points;

	if( !_state_ptr || !_edge || !_count )
	{
		if( _points && *_points )
		{
			free( *_points );
			return TRUE;
		}

		return FALSE;
	}

	state = (LIBDOT_STATE *)_state_ptr;
	if( !state->graph )
		return FALSE;

	spl = ED_spl( (Agedge_t *)_edge ); 
	if ( !spl )
		return FALSE;

	if( !spl->list || spl->size != 1 )
		return FALSE;

	count = spl->list->size + ( spl->list->sflag ? 2 : 0 ) + ( spl->list->eflag ? 2 : 0  );
	
	points = (long *)malloc( sizeof(long) * 2 * count );
	if( !points )
		return FALSE;

	*_count  = count;
	*_points = points;

	if( spl->list->sflag )
	{
		*points++ = ROUND( spl->list->sp.x * (DEFAULT_DPI/POINTS_PER_INCH) );
		*points++ = ROUND( spl->list->sp.y * (DEFAULT_DPI/POINTS_PER_INCH) );

		*points++ = ROUND( spl->list->list[0].x * (DEFAULT_DPI/POINTS_PER_INCH) );
		*points++ = ROUND( spl->list->list[0].y * (DEFAULT_DPI/POINTS_PER_INCH) );
	}

	for( i=0 ; i<spl->list->size ; i++ )
	{
		*points++ = ROUND( spl->list->list[i].x * (DEFAULT_DPI/POINTS_PER_INCH) );
		*points++ = ROUND( spl->list->list[i].y * (DEFAULT_DPI/POINTS_PER_INCH) );
	}

	if( spl->list->eflag )
	{
		*points++ = ROUND( spl->list->list[spl->list->size - 1].x * (DEFAULT_DPI/POINTS_PER_INCH) );
		*points++ = ROUND( spl->list->list[spl->list->size - 1].y * (DEFAULT_DPI/POINTS_PER_INCH) );

		*points++ = ROUND( spl->list->ep.x * (DEFAULT_DPI/POINTS_PER_INCH) );
		*points++ = ROUND( spl->list->ep.y * (DEFAULT_DPI/POINTS_PER_INCH) );
	}

	return TRUE;
}
//---------------------------------------------------------------------------
