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

using Microsoft.VisualBasic.ApplicationServices;

namespace Graphviz
{
	public class Program : WindowsFormsApplicationBase
	{
		protected override bool OnStartup(StartupEventArgs eventArgs)
		{
			/* if no files opened from the Explorer, pose the open file dialog to get them, then open the lot */
			ICollection<string> filesToOpen = eventArgs.CommandLine.Count == 0 ?
				(ICollection<string>)FormController.Instance.FilesToOpen() :
				(ICollection<string>)eventArgs.CommandLine;
			if (filesToOpen != null) {
				MainForm = FormController.Instance.OpenFiles(filesToOpen);
				return base.OnStartup(eventArgs);
			}
			else
				/* user cancelled open dialog, so just quit */
				return false;
		}
		
		protected override void OnStartupNextInstance(StartupNextInstanceEventArgs eventArgs)
		{
			/* if some files opened from the Explorer, open them */
			if (eventArgs.CommandLine.Count > 0)
				MainForm = FormController.Instance.OpenFiles(eventArgs.CommandLine);
			base.OnStartupNextInstance(eventArgs);
		}
		
		private Program()
		{
			EnableVisualStyles = true;
			IsSingleInstance = true;
			ShutdownStyle = ShutdownMode.AfterAllFormsClose;
		}
		
		[STAThread]
		static void Main(string[] commandLine)
		{
			new Program().Run(commandLine);
		}
	}
}
