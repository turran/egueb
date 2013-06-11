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

#ifndef _EGUEB_SVG_PRIVATE_INPUT_H_
#define _EGUEB_SVG_PRIVATE_INPUT_H_

typedef Egueb_Dom_Node * (*Egueb_Svg_Input_Element_At)(void *data, int x, int y);

typedef struct _Egueb_Svg_Input Egueb_Svg_Input;
typedef struct _Egueb_Svg_Input_Descriptor
{
	Egueb_Svg_Input_Element_At element_at;
} Egueb_Svg_Input_Descriptor;

Egueb_Svg_Input * egueb_svg_input_new(Egueb_Svg_Input_Descriptor *descriptor, void *data);
void egueb_svg_input_free(Egueb_Svg_Input *thiz);
void egueb_svg_input_feed_mouse_move(Egueb_Svg_Input *thiz, int x, int y);
void egueb_svg_input_feed_mouse_down(Egueb_Svg_Input *thiz, int button);
void egueb_svg_input_feed_mouse_up(Egueb_Svg_Input *thiz, int button);

#endif

