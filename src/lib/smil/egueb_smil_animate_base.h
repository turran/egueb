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
#ifndef _EGUEB_SMIL_ANIMATE_BASE_H_
#define _EGUEB_SMIL_ANIMATE_BASE_H_

EAPI void egueb_smil_animate_base_to_set(Egueb_Dom_Node *n, Egueb_Dom_String *v);
EAPI Egueb_Dom_String * egueb_smil_animate_base_to_get(Egueb_Dom_Node *n);
EAPI void egueb_smil_animate_base_from_set(Egueb_Dom_Node *n, Egueb_Dom_String *v);
EAPI Egueb_Dom_String * egueb_smil_animate_base_from_get(Egueb_Dom_Node *n);

#if 0
EAPI Eina_Bool egueb_smil_is_animate_base(Ender_Element *e);
EAPI void egueb_smil_animate_base_values_set(Ender_Element *e, const char *v);
EAPI void egueb_smil_animate_base_values_get(Ender_Element *e, const char **v);
EAPI void egueb_smil_animate_base_calc_mode_set(Ender_Element *e, Egueb_Smil_Calc_Mode calc_mode);
EAPI void egueb_smil_animate_base_calc_mode_get(Ender_Element *e, Egueb_Smil_Calc_Mode *calc_mode);
EAPI void egueb_smil_animate_base_key_times_set(Ender_Element *e, const char *v);
EAPI void egueb_smil_animate_base_key_times_get(Ender_Element *e, const char **v);
EAPI void egueb_smil_animate_base_key_splines_set(Ender_Element *e, const char *v);
EAPI void egueb_smil_animate_base_key_splines_get(Ender_Element *e, const char **v);
#endif

#endif
