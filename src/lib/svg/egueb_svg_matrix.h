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

#ifndef _EGUEB_SVG_MATRIX_H
#define _EGUEB_SVG_MATRIX_H

/**
 * @defgroup Egueb_Svg_Matrix_Group Matrix
 * @brief Matrix type
 * @ingroup Egueb_Svg_Type_Group
 * @{
 */

typedef Enesim_Matrix Egueb_Svg_Matrix;

typedef struct _Egueb_Svg_Matrix_Animated
{
	Enesim_Matrix base;
	Enesim_Matrix anim;
} Egueb_Svg_Matrix_Animated;

EAPI extern const Egueb_Svg_Matrix EGUEB_SVG_MATRIX_IDENTITY;

/**
 * @}
 */

#endif
