/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#ifndef _ESVG_ANIMATE_BASE_H
#define _ESVG_ANIMATE_BASE_H

EAPI Eina_Bool esvg_is_animate_base(Ender_Element *e);
EAPI void esvg_animate_base_to_set(Ender_Element *e, const char *v);
EAPI void esvg_animate_base_to_get(Ender_Element *e, const char **v);
EAPI void esvg_animate_base_from_set(Ender_Element *e, const char *v);
EAPI void esvg_animate_base_from_get(Ender_Element *e, const char **v);
EAPI void esvg_animate_base_values_set(Ender_Element *e, const char *v);
EAPI void esvg_animate_base_values_get(Ender_Element *e, const char **v);
EAPI void esvg_animate_base_calc_mode_set(Ender_Element *e, Esvg_Calc_Mode calc_mode);
EAPI void esvg_animate_base_calc_mode_get(Ender_Element *e, Esvg_Calc_Mode *calc_mode);
EAPI void esvg_animate_base_key_times_set(Ender_Element *e, const char *v);
EAPI void esvg_animate_base_key_times_get(Ender_Element *e, const char **v);
EAPI void esvg_animate_base_key_splines_set(Ender_Element *e, const char *v);
EAPI void esvg_animate_base_key_splines_get(Ender_Element *e, const char **v);

#endif
