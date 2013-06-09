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

#ifndef _EGUEB_SVG_REFERENCE_H_
#define _EGUEB_SVG_REFERENCE_H_

/* FIXME do we really need this reference destroy? */
typedef struct _Egueb_Svg_Reference Egueb_Svg_Reference;
typedef void (*Egueb_Svg_Reference_Destroy_Cb)(Egueb_Svg_Reference *thiz, Egueb_Dom_Node *referencer);
EAPI void egueb_svg_reference_free(Egueb_Svg_Reference *thiz);
EAPI Eina_Bool egueb_svg_reference_process(Egueb_Svg_Reference *thiz);
EAPI void egueb_svg_reference_referencer_get(Egueb_Svg_Reference *thiz,
		Egueb_Dom_Node **referencer);

#endif
