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

#ifndef _EGUEB_SMIL_CALC_MODE_H_
#define _EGUEB_SMIL_CALC_MODE_H_

typedef enum _Egueb_Smil_Calc_Mode
{
	EGUEB_SMIL_CALC_MODE_DISCRETE,
	EGUEB_SMIL_CALC_MODE_LINEAR,
	EGUEB_SMIL_CALC_MODE_PACED,
	EGUEB_SMIL_CALC_MODE_SPLINE,
} Egueb_Smil_Calc_Mode;

EAPI const Egueb_Dom_Value_Descriptor * egueb_smil_calc_mode_descriptor_get(void);

#endif

