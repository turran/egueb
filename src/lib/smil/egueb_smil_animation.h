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

#ifndef _EGUEB_SMIL_ANIMATION_H_
#define _EGUEB_SMIL_ANIMATION_H_

EAPI Eina_Bool egueb_smil_is_animation(Egueb_Dom_Node *n);
EAPI Etch * egueb_smil_animation_etch_get(Egueb_Dom_Node *n);
EAPI void egueb_smil_animation_etch_set(Egueb_Dom_Node *n, Etch *etch);

#if 0
EAPI void egueb_smil_element_animation_attribute_name_set(Ender_Element *e, const char *name);
EAPI void egueb_smil_element_animation_attribute_name_get(Ender_Element *e, const char **name);
EAPI void egueb_smil_element_animation_attribute_type_set(Ender_Element *e, Egueb_Smil_Attribute_Type type);
EAPI void egueb_smil_element_animation_attribute_type_get(Ender_Element *e, Egueb_Smil_Attribute_Type *type);
EAPI void egueb_smil_element_animation_dur_set(Ender_Element *e, Egueb_Smil_Duration *v);
EAPI void egueb_smil_element_animation_dur_get(Ender_Element *e, Egueb_Smil_Duration *v);
EAPI void egueb_smil_element_animation_additive_set(Ender_Element *e, Egueb_Smil_Additive additive);
EAPI void egueb_smil_element_animation_additive_get(Ender_Element *e, Egueb_Smil_Additive *additive);
EAPI void egueb_smil_element_animation_accumulate_set(Ender_Element *e, Egueb_Smil_Accumulate accumulate);
EAPI void egueb_smil_element_animation_accumulate_get(Ender_Element *e, Egueb_Smil_Accumulate *accumulate);
EAPI void egueb_smil_element_animation_repeat_count_set(Ender_Element *e, int repeat_count);
EAPI void egueb_smil_element_animation_repeat_count_get(Ender_Element *e, int *repeat_count);

EAPI void egueb_smil_element_animation_begin_set(Ender_Element *e, Eina_List *begin);
EAPI void egueb_smil_element_animation_end_set(Ender_Element *e, Eina_List *end);
EAPI void egueb_smil_element_animation_fill_set(Ender_Element *e, Egueb_Smil_Fill fill);
EAPI void egueb_smil_element_animation_fill_get(Ender_Element *e, Egueb_Smil_Fill *fill);
#endif

#endif

