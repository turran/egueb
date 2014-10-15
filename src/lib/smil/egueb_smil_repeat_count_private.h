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

#ifndef _EGUEB_SMIL_REPEAT_COUNT_PRIVATE_H_
#define _EGUEB_SMIL_REPEAT_COUNT_PRIVATE_H_

char * egueb_smil_repeat_count_string_to(Egueb_Smil_Repeat_Count *thiz);
Eina_Bool egueb_smil_repeat_count_string_from(Egueb_Smil_Repeat_Count *thiz, const char *str);
void egueb_smil_repeat_count_interpolate(Egueb_Smil_Repeat_Count *v,
		Egueb_Smil_Repeat_Count *a, Egueb_Smil_Repeat_Count *b, double m,
		Egueb_Smil_Repeat_Count *add, Egueb_Smil_Repeat_Count *acc, int mul);

#endif
