//---------------------------------------------------------------------------
#ifndef LIBDOT_H
#define LIBDOT_H
//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

#ifdef LIBDOT_DLL_LINK
	#define LIBDOT_EXTERN		__declspec( dllexport )

	#ifndef BOOL
	typedef int BOOL;
	#endif
#else
	#define LIBDOT_EXTERN		extern __declspec( dllimport )
#endif

	
// general...

LIBDOT_EXTERN void * libdot_Init( void );

LIBDOT_EXTERN void libdot_Cleanup( void * _state_ptr );

// graph...

// Note: Default attributes: rankdir=BT, splines=polyline
LIBDOT_EXTERN BOOL libdot_GraphCreate( void * _state_ptr );

LIBDOT_EXTERN BOOL libdot_GraphDestroy( void * _state_ptr );

LIBDOT_EXTERN BOOL libdot_GraphLayout( void * _state_ptr );

LIBDOT_EXTERN BOOL libdot_GraphPosition( void * _state_ptr, long * _width, long * _height );

LIBDOT_EXTERN void libdot_GraphSetAttribute( void * _state_ptr, char * _name, char * _value );

// node...

// Note: Default attributes: fixedsize=true, shape=rectangle
LIBDOT_EXTERN void * libdot_NodeCreate( void * _state_ptr, long _width, long _height );

LIBDOT_EXTERN BOOL libdot_NodePosition( void * _state_ptr, void * _node, long * _x, long * _y, long * _width, long * _height );

LIBDOT_EXTERN void libdot_NodeSetAttribute( void * _state_ptr, void * _node, char * _name, char * _value );

// edge...

// Note: Default attributes: headport=s, tailport=n
LIBDOT_EXTERN void * libdot_EdgeCreate( void * _state_ptr, void * _origin_node, void * _dest_node );

LIBDOT_EXTERN BOOL libdot_EdgePosition( void * _state_ptr, void * _edge, long * count, long ** points );

LIBDOT_EXTERN void libdot_EdgeSetAttribute( void * _state_ptr, void * _edge, char * _name, char * _value );


#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------