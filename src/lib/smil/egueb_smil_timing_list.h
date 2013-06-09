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

#ifndef _EGUEB_SMIL_TIMING_LIST_H_
#define _EGUEB_SMIL_TIMING_LIST_H_

EAPI const Egueb_Dom_Value_Descriptor * egueb_smil_timing_list_descriptor_get(void);
EAPI Eina_Bool egueb_smil_timing_list_string_from(Egueb_Dom_List *l, const char *str);
EAPI char * egueb_smil_timing_list_string_to(Egueb_Dom_List *l);

#endif
