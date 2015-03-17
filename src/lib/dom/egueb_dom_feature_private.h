/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _EGUEB_DOM_FEATURE_PRIVATE_H_
#define _EGUEB_DOM_FEATURE_PRIVATE_H_

struct _Egueb_Dom_Feature
{
	Enesim_Object_Instance parent;
	int ref;
};

typedef struct _Egueb_Dom_Feature_Class
{
	Enesim_Object_Class parent;
} Egueb_Dom_Feature_Class;

/* FIXME We need to export this function for correct linking */
EAPI Enesim_Object_Descriptor * egueb_dom_feature_descriptor_get(void);

#define EGUEB_DOM_FEATURE_DESCRIPTOR egueb_dom_feature_descriptor_get()

#define EGUEB_DOM_FEATURE_CLASS_GET(o) EGUEB_DOM_FEATURE_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_FEATURE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,			\
		Egueb_Dom_Feature_Class, EGUEB_DOM_FEATURE_DESCRIPTOR)
#define EGUEB_DOM_FEATURE(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Feature, EGUEB_DOM_FEATURE_DESCRIPTOR)

#endif
