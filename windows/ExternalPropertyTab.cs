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
using System.Drawing;
using System.Windows.Forms.Design;

namespace Graphviz
{
	public class ExternalPropertyTab : PropertyTab
	{
		public ExternalPropertyTab(string name, Bitmap bitmap, PropertyDescriptorCollection externalProperties)
		{
			_name = name;
			_bitmap = bitmap;
			_externalProperties = externalProperties;
		}

		public override PropertyDescriptorCollection GetProperties(object component, Attribute[] attributes)
		{
			if (attributes == null || attributes.Length == 0)
				/* no need to filter by attribute, just return all properties */
				return _externalProperties;
			else {
				/* filter in properties that match all given attributes */
				PropertyDescriptorCollection properties = new PropertyDescriptorCollection(new PropertyDescriptor[0]);
				foreach (PropertyDescriptor property in _externalProperties) {
					bool allMatch = true;
					foreach (Attribute attribute in attributes)
						if (!property.Attributes[attribute.GetType()].Match(attribute)) {
							allMatch = false;
							break;
						}
					if (allMatch)
						properties.Add(property);
				}
				return properties;
			}
			
		}

		public override bool CanExtend(object extendee)
		{
			return true;
		}

		public override string TabName
		{
			get { return _name; }
		}

		// Provides an image for the property tab.
		public override Bitmap Bitmap
		{
			get { return _bitmap; }
		}

		private readonly string _name;
		private readonly Bitmap _bitmap;
		private readonly PropertyDescriptorCollection _externalProperties;
	}
}
