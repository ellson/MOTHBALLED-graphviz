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
using System.ComponentModel;
using System.IO;
using System.Threading;

namespace Graphviz
{
	public class PathWatcher: IDisposable
	{
		public event CancelEventHandler Changed;
		
		public ISynchronizeInvoke SynchronizingObject
		{
			get { return _synchronizingObject; }
			set { _synchronizingObject = value; }
		}
		
		public string Watched
		{
			get { return _watched; }
		}
		
		public PathWatcher(string watched)
		{
			_watched = watched;
			_synchronizingObject = null;
			
			_lastChange = DateTime.MinValue;
			_checker = new Timer(delegate(object state)
			{
				_synchronizingObject.BeginInvoke((ThreadStart)delegate()
				{
					if (Changed != null)
					{
						/* debounce file watch changes: if last write time hadn't actually changed, don't bother notifying */
						DateTime thisChange = File.GetLastWriteTimeUtc(_watched);
						if (_lastChange != thisChange)
						{
							/* if any registered delegate didn't cancel the event, ask checker to check it again after a while */
							CancelEventArgs eventArgs = new CancelEventArgs(true);
							Changed(this, eventArgs);
							if (eventArgs.Cancel)
								_lastChange = thisChange;
							else
								_checker.Change(RECHECK_DELAY, Timeout.Infinite);
						}
					}
				},
				new object[0]);
			});

			_watcher = new FileSystemWatcher(Path.GetDirectoryName(watched), Path.GetFileName(watched));
			_watcher.NotifyFilter = NotifyFilters.FileName | NotifyFilters.LastWrite;
			_watcher.Changed += delegate(object sender, FileSystemEventArgs eventArgs)
			{
				/* check now */
				_checker.Change(0, Timeout.Infinite);
			};
			_watcher.Created += delegate(object sender, FileSystemEventArgs eventArgs)
			{
				/* check now */
				_checker.Change(0, Timeout.Infinite);
			};
			_watcher.Renamed += delegate(object sender, RenamedEventArgs eventArgs)
			{
				/* if file got changed to our name, check now */
				if (eventArgs.Name == _watcher.Filter)
					_checker.Change(0, Timeout.Infinite);
			};
		}
		
		public void Start()
		{
			_watcher.EnableRaisingEvents = true;
		}
		
		void IDisposable.Dispose()
		{
			_checker.Dispose();
		}
		
		private const int RECHECK_DELAY = 100;
		
		private readonly string _watched;
		private ISynchronizeInvoke _synchronizingObject;

		private DateTime _lastChange;
		private readonly Timer _checker;
		private readonly FileSystemWatcher _watcher;

	}
}
