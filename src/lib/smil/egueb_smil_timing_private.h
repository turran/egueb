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

#ifndef _EGUEB_SMIL_TIMING_PRIVATE_H_
#define _EGUEB_SMIL_TIMING_PRIVATE_H_

Eina_Bool egueb_smil_timing_string_from(Egueb_Smil_Timing *thiz, const char *s);
char * egueb_smil_timing_string_to(Egueb_Smil_Timing *thiz);
void egueb_smil_timing_copy(const Egueb_Smil_Timing *thiz, Egueb_Smil_Timing *copy, Eina_Bool full);
void egueb_smil_timing_interpolate(Egueb_Smil_Timing *v,
		Egueb_Smil_Timing *a, Egueb_Smil_Timing *b, double m,
		Egueb_Smil_Timing *add, Egueb_Smil_Timing *acc, int mul);

#endif

