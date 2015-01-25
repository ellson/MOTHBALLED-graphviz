libdot
======

Libdot is a standalone x86 or x64 DLL with no dependencies and a simple API to interface with the DOT layout algorithm. No support for rendering a graph is provided.

Example
=======

	do
	{
		// initialize libdot...
		
		void * state = libdot_Init();
		if( !state )
			break;

		// create a graph to work with...
		
		if( !libdot_GraphCreate( state ) )
			break;

		libdot_GraphSetAttribute( state, "rankdir", "TD" );

		libdot_GraphSetAttribute( state, "splines", "polyline" );

		// create some nodes...

		void * nodeA = libdot_NodeCreate( state, 200, 500 );
		if( !nodeA )
			break;
			
		libdot_NodeSetAttribute( state, nodeA, "fixedsize", "true" );
		
		libdot_NodeSetAttribute( state, nodeA, "shape", "rectangle" );

		void * nodeB = libdot_NodeCreate( state, 200, 200 );
		if( !nodeB )
			break;
			
		libdot_NodeSetAttribute( state, nodeB, "fixedsize", "true" );
		
		libdot_NodeSetAttribute( state, nodeB, "shape", "rectangle" );

		// create some edges...

		void * edgeAB = libdot_EdgeCreate( state, nodeA, nodeB );
		if( !edgeAB )
			break;
		
		libdot_EdgeSetAttribute( state, edgeAB, "headport", "s" );
		
		libdot_EdgeSetAttribute( state, edgeAB, "tailport", "n" );

		// perform the DOT layout...
		
		if( !libdot_GraphLayout( state ) )
			break;

		// get the node positions after layout...

		long x, y, width, height;

		if( !libdot_NodePosition( state, nodeA, &x, &y, &width, &height ) )
			break;

		// we could now draw node A...
		
		if( !libdot_NodePosition( state, nodeB, &x, &y, &width, &height ) )
			break;
			
		// we could now draw node B...
		
		// get the edge points after layout...

		long count           = 0;
		long * points        = NULL;

		if( !libdot_EdgePosition( state, edgeAB, &count, &points ) || !points )
			break;

		for( long index=0 ; index<count ; index++ )
		{
			long x = points[index];
			long y = points[index+1];
			
			// we could draw this edge
		}

		// free the list of points...
		libdot_EdgePosition( NULL, NULL, NULL, &points );

	} while( 0 );

	if( state )
		libdot_Cleanup( state );
    
