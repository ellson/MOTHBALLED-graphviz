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

namespace Graphviz
{
	public partial class GraphForm : Form
	{
		public ToolStripMenuItem OpenMenuItem
		{
			get { return openToolStripMenuItem; }
		}
		
		public ToolStripMenuItem WindowMenuItem
		{
			get { return windowToolStripMenuItem; }
		}
		
		public GraphForm(string filename)
		{
			InitializeComponent();
		
			_graph = new Graph(filename);
			Text = Path.GetFileName(filename);
			_graph.Layout("dot");
			using (Stream stream = _graph.Render("emfplus:gdiplus"))
				graphControl.Image = new Metafile(stream);
		}

		protected override void OnFormClosed(FormClosedEventArgs e)
		{
			((IDisposable)_graph).Dispose();
			base.OnFormClosed(e);
		}
		
		private void exitToolStripMenuItem_Click(object sender, EventArgs e)
		{
			Application.Exit();
		}
		
		private readonly Graph _graph;
	}
}