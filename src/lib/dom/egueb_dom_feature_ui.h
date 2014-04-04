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
#ifndef _EGUEB_DOM_FEATURE_UI_H_
#define _EGUEB_DOM_FEATURE_UI_H_

EAPI extern Egueb_Dom_String *EGUEB_DOM_FEATURE_UI_NAME;

typedef Egueb_Dom_Input * (*Egueb_Dom_Feature_UI_Descriptor_Input_Get)(Egueb_Dom_Node *n);

typedef struct _Egueb_Dom_Feature_UI_Descriptor
{
	Egueb_Dom_Feature_UI_Descriptor_Input_Get input_get;
} Egueb_Dom_Feature_UI_Descriptor;

EAPI Eina_Bool egueb_dom_feature_ui_input_get(Egueb_Dom_Feature *f, Egueb_Dom_Input **i);
EAPI Eina_Bool egueb_dom_feature_ui_add(Egueb_Dom_Node *n,
		const Egueb_Dom_Feature_UI_Descriptor *d);

#endif

