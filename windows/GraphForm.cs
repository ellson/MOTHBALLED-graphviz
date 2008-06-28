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
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Text;

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
			
			/* set up export dialog with initial file and filters from the graph devices */
			StringBuilder exportFilter = new StringBuilder();
			int filterIndex = 0;
			for (int deviceIndex = 0; deviceIndex < _devices.Count; ++deviceIndex)
			{
				if (deviceIndex > 0)
					exportFilter.Append("|");
					
				string device = _devices[deviceIndex];
				exportFilter.Append(String.Format("{0} (*.{1})|*.{1}", device.ToUpper(), device));
				
				if (filterIndex == 0 && device == "emfplus")
					filterIndex = deviceIndex;
			}
			exportFileDialog.FileName = Path.GetFileNameWithoutExtension(fileName);
			exportFileDialog.Filter = exportFilter.ToString();
			exportFileDialog.FilterIndex = filterIndex + 1;
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

		ToolStripMenuItem FormController.IMenus.WindowMenuItem
		{
			get { return windowToolStripMenuItem; }
		}
		
		private void ShowAttributes_Click(object sender, EventArgs eventArgs)
		{
			AttributeInspectorForm.Instance.Show();
		}

		private void ZoomToFit_Click(object sender, EventArgs eventArgs)
		{
			graphControl.ZoomToFit();
		}
		
		private void ActualSize_Click(object sender, EventArgs eventArgs)
		{
			graphControl.ActualSize();
		}
		
		private void ZoomIn_Click(object sender, EventArgs eventArgs)
		{
			graphControl.ZoomIn();
		}

		private void ZoomOut_Click(object sender, EventArgs eventArgs)
		{
			graphControl.ZoomOut();
		}

		private void exportToolStripMenuItem_Click(object sender, EventArgs e)
		{
			/* pose the export dialog and then render the graph with the selected file and format */
			if (exportFileDialog.ShowDialog(this) == DialogResult.OK)
			{
				int filterIndex = exportFileDialog.FilterIndex - 1;
				if (filterIndex >= 0 && filterIndex < _devices.Count)
					_graph.Render(_devices[filterIndex], exportFileDialog.FileName);
			}
		}
		
		private static readonly IList<string> _devices = Graph.GetPlugins(Graph.API.Device, false);
		private readonly Graph _graph;

	}
}
