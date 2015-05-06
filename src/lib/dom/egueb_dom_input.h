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

#ifndef _EGUEB_DOM_INPUT_H_
#define _EGUEB_DOM_INPUT_H_

typedef struct _Egueb_Dom_Input Egueb_Dom_Input;

/* All the returned nodes *must* be part of the document, that is,
 * they must be added with a topmost parent of type document
 */
typedef Egueb_Dom_Node * (*Egueb_Dom_Input_Descriptor_Element_At)(Egueb_Dom_Node *current,
		int x, int y, void *data);
typedef Egueb_Dom_Node * (*Egueb_Dom_Input_Descriptor_Focus_Next)(Egueb_Dom_Node *current, void *data);
typedef Egueb_Dom_Node * (*Egueb_Dom_Input_Descriptor_Focus_Prev)(Egueb_Dom_Node *current, void *data);

#define EGUEB_DOM_INPUT_DESCRIPTOR_VERSION 0
typedef struct _Egueb_Dom_Input_Descriptor
{
	int version;
	Egueb_Dom_Input_Descriptor_Element_At element_at;
	Egueb_Dom_Input_Descriptor_Focus_Next focus_next;
	Egueb_Dom_Input_Descriptor_Focus_Prev focus_prev;
} Egueb_Dom_Input_Descriptor;

EAPI Egueb_Dom_Input * egueb_dom_input_new(Egueb_Dom_Input_Descriptor *descriptor, void *data);
EAPI void egueb_dom_input_unref(Egueb_Dom_Input *thiz);
EAPI Egueb_Dom_Input * egueb_dom_input_ref(Egueb_Dom_Input *thiz);

EAPI void egueb_dom_input_mouse_position_get(Egueb_Dom_Input *thiz,
		int *x, int *y);
EAPI void egueb_dom_input_feed_mouse_move(Egueb_Dom_Input *thiz, int x, int y);
EAPI void egueb_dom_input_feed_mouse_down(Egueb_Dom_Input *thiz, int button);
EAPI void egueb_dom_input_feed_mouse_up(Egueb_Dom_Input *thiz, int button);
EAPI void egueb_dom_input_feed_mouse_wheel(Egueb_Dom_Input *thiz, int deltax, int deltay, int deltaz);
EAPI void egueb_dom_input_feed_key_down(Egueb_Dom_Input *thiz,
		Egueb_Dom_String *key);
EAPI void egueb_dom_input_feed_key_up(Egueb_Dom_Input *thiz,
		Egueb_Dom_String *key);

#endif

