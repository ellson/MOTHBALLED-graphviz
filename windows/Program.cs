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
using System.Collections.ObjectModel;
using System.IO;
using System.Windows.Forms;

using Microsoft.VisualBasic.ApplicationServices;

namespace Graphviz
{
	public class Program : WindowsFormsApplicationBase
	{
		protected override bool OnStartup(StartupEventArgs eventArgs)
		{
			/* if no files opened from the Explorer, pose the open file dialog to get them, then open the lot */
			ICollection<string> filesToOpen = eventArgs.CommandLine.Count == 0 ?
				(ICollection<string>)FilesToOpen() :
				(ICollection<string>)eventArgs.CommandLine;
			if (filesToOpen != null) {
				OpenFiles(filesToOpen);
				return base.OnStartup(eventArgs);
			}
			else
				return false;
		}
		
		protected override void OnStartupNextInstance(StartupNextInstanceEventArgs eventArgs)
		{
			/* if some files opened from the Explorer, open them */
			if (eventArgs.CommandLine.Count > 0)
				OpenFiles(eventArgs.CommandLine);
			base.OnStartupNextInstance(eventArgs);
		}
		
		private string[] FilesToOpen()
		{
			/* lazily initialize open file dialog... sometimes we are created only to pass args to the main instance */
			if (_openFileDialog == null)
			{
				_openFileDialog = new OpenFileDialog();
				_openFileDialog.Filter = "Graphviz graphs (*.dot)|*.dot|All files (*.*)|*.*";
				_openFileDialog.Multiselect = true;
			}
			
			/* if user said OK, return the files he selected */
			return _openFileDialog.ShowDialog() == DialogResult.OK ? _openFileDialog.FileNames : null;
		}
		
		private void OpenFiles(ICollection<string> filenames)
		{
			Form foundForm = null;
			foreach (string filename in filenames) {
				string canonicalFilename = Path.GetFullPath(filename).ToLower();
				if (_documentForms.ContainsKey(canonicalFilename))
					foundForm = _documentForms[canonicalFilename];
				else {
					GraphForm newForm = new GraphForm(filename);
					_documentForms[canonicalFilename] = foundForm = newForm;

					/* when the form closes, remove it from document form list */
					newForm.FormClosed += delegate(object sender, FormClosedEventArgs eventArgs)
					{
						_documentForms.Remove(canonicalFilename);
					};
					
					/* clicking the Open menu item calls our Open method */
					newForm.OpenMenuItem.Click += delegate(object sender, EventArgs eventArgs)
					{
						string[] filesToOpen = FilesToOpen();
						if (filesToOpen != null)
							OpenFiles(filesToOpen);
					};

					/* compose the Window menu out of all the open form titles */
					newForm.WindowMenuItem.DropDownOpening += delegate(object sender, EventArgs eventArgs)
					{
						ToolStripMenuItem windowMenuItem = sender as ToolStripMenuItem;
						if (windowMenuItem != null) {
							windowMenuItem.DropDownItems.Clear();
							int i = 0;
							foreach (Form form in OpenForms) {
								Form innerForm = form;
								ToolStripMenuItem formMenuItem = new ToolStripMenuItem(string.Format("{0} {1}", ++i, form.Text));
								formMenuItem.Checked = Form.ActiveForm == innerForm;
								formMenuItem.Click += delegate(object innerSender, EventArgs innerEventArgs)
								{
									innerForm.Activate();
								};
								windowMenuItem.DropDownItems.Add(formMenuItem);
							}
						}
					};
					foundForm.Show();
				}
			}
			
			MainForm = foundForm;
		
		}
		
		private Program()
		{
			EnableVisualStyles = true;
			IsSingleInstance = true;
			ShutdownStyle = ShutdownMode.AfterAllFormsClose;
			
			_openFileDialog = null;
			_documentForms = new Dictionary<string, Form>();
		}
		
		[STAThread]
		static void Main(string[] commandLine)
		{
			new Program().Run(commandLine);
		}
		
		private OpenFileDialog _openFileDialog;
		private readonly IDictionary<string, Form> _documentForms;
	}
}