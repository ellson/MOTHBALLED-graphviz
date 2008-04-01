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
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;
using System.Collections.Generic;

namespace Graphviz
{
	public partial class GraphForm : Form, FormController.IMenus
	{
		public Graph Graph
		{
			get { return _graph; }
		}
		
		public GraphForm(string fileName)
		{
			InitializeComponent();
		
			_graph = new Graph(fileName);
		
			/* whenever graph changes, rerender and display the graph */
			_graph.Changed += delegate(object sender, EventArgs e)
		{
			using (Stream stream = _graph.Render("emfplus:gdiplus"))
				graphControl.Image = new Metafile(stream);
			};
			_graph.Arguments["layout"] = "dot";
		}

		protected override void OnFormClosed(FormClosedEventArgs e)
		{
			/* when form closes, clean up graph too */
			((IDisposable)_graph).Dispose();
			base.OnFormClosed(e);
		}

		ToolStripMenuItem FormController.IMenus.ExitMenuItem
		{
			get { return exitToolStripMenuItem; }
		}

		ToolStripMenuItem FormController.IMenus.OpenMenuItem
		{
			get { return openToolStripMenuItem; }
		}

		ToolStripMenuItem FormController.IMenus.ShowAttributesMenuItem
		{
			get { return showAttributesToolStripMenuItem; }
		}

		ToolStripMenuItem FormController.IMenus.WindowMenuItem
		{
			get { return windowToolStripMenuItem; }
		}
		
		private readonly Graph _graph;

	}
}
