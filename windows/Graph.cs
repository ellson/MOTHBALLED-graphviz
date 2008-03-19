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

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;

namespace Graphviz
{
	public class Graph : IDisposable
	{
		public class Exception : ApplicationException
		{
			public Exception(string message): base(message)
			{
			}
		}
		
		public Graph(string filename)
		{
			IntPtr file = fopen(filename, "r");
			if (file == IntPtr.Zero)
				throw new Win32Exception();
			_graph = agread(file);
			if (_graph == IntPtr.Zero)
				throw new Win32Exception();
			fclose(file);
		}
		
		public void Save(string filename)
		{
			IntPtr file = fopen(filename, "w");
			if (file == IntPtr.Zero)
				throw new Win32Exception();
			if (agwrite(_graph, file) != 0)
				throw new Win32Exception();
			fclose(file);
		}
		
		public void Layout(string engine)
		{
			if (gvLayout(_context, _graph, engine) != 0)
				throw new Exception("bad layout");
		}
		
		public Stream Render(string format)
		{
			unsafe {
				byte* result;
				uint length;
				if (gvRenderData(_context, _graph, format, out result, out length) != 0)
					throw new Exception("bad render");
				return new RenderStream(result, length);
			}
		}
		
		void IDisposable.Dispose()
		{
 			agclose(_graph);
		}
		
		private unsafe class RenderStream : UnmanagedMemoryStream
		{
			public RenderStream(byte* pointer, long length): base(pointer, length)
			{
				_pointer = pointer;
			}

			protected override void Dispose(bool disposing)
			{
				base.Dispose(disposing);
				if (disposing)
					free(_pointer);
			}
			
			private readonly byte* _pointer;
		}
		
		[DllImport("libgraph-4.dll", SetLastError = true)]
		private static extern void agclose(IntPtr file);

		[DllImport("libgraph-4.dll", SetLastError = true)]
		private static extern IntPtr agread(IntPtr file);

		[DllImport("libgraph-4.dll", SetLastError = true)]
		private static extern int agwrite(IntPtr graph, IntPtr file);
		
		[DllImport("libgvc-4.dll")]
		private static extern IntPtr gvContext();

		[DllImport("libgvc-4.dll")]
		private static extern int gvLayout(IntPtr context, IntPtr graph, string engine);

		[DllImport("libgvc-4.dll")]
		private static extern int gvRenderFilename(IntPtr context, IntPtr graph, string format, string filename);
		
		[DllImport("libgvc-4.dll")]
		private static extern unsafe int gvRenderData(IntPtr context, IntPtr graph, string format, out byte* result, out uint length);

		[DllImport("msvcrt.dll", SetLastError = true)]
		private static extern int fclose(IntPtr file);

		[DllImport("msvcrt.dll", SetLastError = true)]
		private static extern IntPtr fopen(string filename, string mode);

		[DllImport("msvcrt.dll", SetLastError = true)]
		private static extern unsafe void free(byte* pointer);

		private static readonly IntPtr _context = gvContext();
		private readonly IntPtr _graph;
	}
}
