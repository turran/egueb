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

typedef Egueb_Dom_Node * (*Egueb_Dom_Input_Element_At)(void *data, int x, int y);

typedef struct _Egueb_Dom_Input Egueb_Dom_Input;
typedef struct _Egueb_Dom_Input_Descriptor
{
	Egueb_Dom_Input_Element_At element_at;
} Egueb_Dom_Input_Descriptor;

EAPI Egueb_Dom_Input * egueb_dom_input_new(Egueb_Dom_Input_Descriptor *descriptor, void *data);
EAPI void egueb_dom_input_free(Egueb_Dom_Input *thiz);
EAPI void egueb_dom_input_feed_mouse_move(Egueb_Dom_Input *thiz, int x, int y);
EAPI void egueb_dom_input_feed_mouse_down(Egueb_Dom_Input *thiz, int button);
EAPI void egueb_dom_input_feed_mouse_up(Egueb_Dom_Input *thiz, int button);
EAPI void egueb_dom_input_feed_mouse_wheel(Egueb_Dom_Input *thiz, int deltax, int deltay, int deltaz);

#endif

