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

#ifndef _EGUEB_DOM_FEATURE_EXTERNAL_H_
#define _EGUEB_DOM_FEATURE_EXTERNAL_H_

typedef int (*Egueb_Dom_Feature_External_Descriptor_Version_Get_Cb)(void *data);
typedef void (*Egueb_Dom_Feature_External_Descriptor_Init_Cb)(void *data);
typedef void (*Egueb_Dom_Feature_External_Descriptor_Deinit_Cb)(
		void *data);
typedef const char * (*Egueb_Dom_Feature_External_Descriptor_Type_Get_Cb)(void *data);

#define EGUEB_DOM_FEATURE_EXTERNAL_DESCRIPTOR_VERSION 0
typedef struct _Egueb_Dom_Feature_External_Descriptor
{
	Egueb_Dom_Feature_External_Descriptor_Version_Get_Cb version_get;
	Egueb_Dom_Feature_External_Descriptor_Init_Cb init;
	Egueb_Dom_Feature_External_Descriptor_Deinit_Cb deinit;
	Egueb_Dom_Feature_External_Descriptor_Type_Get_Cb type_get;
} Egueb_Dom_Feature_External_Descriptor;

EAPI void * egueb_dom_feature_external_data_get(Egueb_Dom_Feature *f);
EAPI Egueb_Dom_Feature * egueb_dom_feature_external_new(
		const Egueb_Dom_Feature_External_Descriptor *descriptor,
		void *data);

#endif

