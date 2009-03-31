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

#ifndef VISIOGRAPHIC_H
#define VISIOGRAPHIC_H

#include "types.h"

namespace Visio
{
	/* Fill VDX element */
	
	class Fill
	{
	public:
		Fill(unsigned char red, unsigned char green, unsigned char blue, double transparency);
		
		/* output the fill */
		void Print(GVJ_t* job) const;
		
	private:
		unsigned char _red;
		unsigned char _green;
		unsigned char _blue;
		double _transparency;	/* 0.0 == opaque, 1.0 == transparent */
	};

	/* Line VDX element */
	
	class Line
	{
	public:
		Line(double weight, unsigned char red, unsigned char green, unsigned char blue, unsigned int pattern, unsigned int beginArrow = 0, unsigned int endArrow = 0);
		
		/* output the line */
		void Print(GVJ_t* job) const;
		
	private:
		double _weight;
		unsigned char _red;
		unsigned char _green;
		unsigned char _blue;
		unsigned int _pattern;		/* solid == 1, dashed == 2, dotted == 3 etc. */
		unsigned int _beginArrow;	/* arrow type e.g. 2 is filled arrow head */
		unsigned int _endArrow;		/* arrow type e.g. 2 is filled arrow head */
	};
	
	/* Geom VDX element */
	
	class Geom
	{
	public:
		virtual boxf GetBounds() const = 0;		/* bounding box -- used by node logic */
		virtual pointf GetFirst() const = 0;	/* first point -- used by edge logic */
		virtual pointf GetLast() const = 0;		/* last point -- used by edge logic */
		virtual pointf GetCenter() const = 0;	/* midpoint of the path -- used by text logic */
		
		/* given first (lower left) and last points (upper right), output the geometry */ 
		virtual void Print(GVJ_t* job, pointf first, pointf last) const = 0;
	};
	
	class Ellipse: public Geom
	{
	public:
		Ellipse(pointf* points, bool filled);
		void Print(GVJ_t* job, pointf first, pointf last) const;
		
		boxf GetBounds() const;
		pointf GetFirst() const;
		pointf GetLast() const;
		pointf GetCenter() const;
	
	private:
		bool _filled;
		pointf _points[2];
	};
	
	class Path: public Geom
	{
	public:
		Path(pointf* points, int pointCount);
		~Path();
		
		boxf GetBounds() const;
		pointf GetFirst() const;
		pointf GetLast() const;
		
	protected:
		pointf* _points;
		int _pointCount;
	};
		
	class Bezier: public Path
	{
	public:
		Bezier(pointf* points, int pointCount, bool filled);
		
		pointf GetCenter() const;
		
		void Print(GVJ_t* job, pointf first, pointf last) const;
		
	private:
		bool _filled;
	};
	
	class Polygon: public Path
	{
	public:
		Polygon(pointf* points, int pointCount, bool filled);
		
		pointf GetCenter() const;

		void Print(GVJ_t* job, pointf first, pointf last) const;
		
	private:
		bool _filled;
	};

	class Polyline: public Path
	{
	public:
		Polyline(pointf* points, int pointCount);
		
		pointf GetCenter() const;

		void Print(GVJ_t* job, pointf first, pointf last) const;
	};
	
	/* Line, Fill and Geom details for each Graphviz graphic */
	
	class Graphic
	{
	public:
		static Graphic* CreateEllipse(GVJ_t* job, pointf* A, bool filled);
		static Graphic* CreateBezier(GVJ_t* job, pointf* A, int n, bool arrow_at_start, bool arrow_at_end, bool filled);
		static Graphic* CreatePolygon(GVJ_t* job, pointf* A, int n, bool filled);
		static Graphic* CreatePolyline(GVJ_t* job, pointf* A, int n);
		
		~Graphic();

		boxf GetBounds() const;
		pointf GetFirst() const;
		pointf GetLast() const;
		pointf GetCenter() const;

		void Print(GVJ_t* job, pointf first, pointf last) const;
		
	private:
		Graphic(Line* line, Fill* fill, Geom* geom);
		
		Line* _line;
		Fill* _fill;
		Geom* _geom;
	};
}

#endif