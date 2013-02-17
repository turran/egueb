#ifndef _ESVG_ELEMENT_ANIMATION_H_
#define _ESVG_ELEMENT_ANIMATION_H_

EAPI Eina_Bool esvg_is_animation(Ender_Element *e);
EAPI void esvg_element_animation_attribute_name_set(Ender_Element *e, const char *name);
EAPI void esvg_element_animation_attribute_name_get(Ender_Element *e, const char **name);
EAPI void esvg_element_animation_attribute_type_set(Ender_Element *e, Esvg_Attribute_Type type);
EAPI void esvg_element_animation_attribute_type_get(Ender_Element *e, Esvg_Attribute_Type *type);
EAPI void esvg_element_animation_dur_set(Ender_Element *e, Esvg_Duration *v);
EAPI void esvg_element_animation_dur_get(Ender_Element *e, Esvg_Duration *v);
EAPI void esvg_element_animation_additive_set(Ender_Element *e, Esvg_Additive additive);
EAPI void esvg_element_animation_additive_get(Ender_Element *e, Esvg_Additive *additive);
EAPI void esvg_element_animation_accumulate_set(Ender_Element *e, Esvg_Accumulate accumulate);
EAPI void esvg_element_animation_accumulate_get(Ender_Element *e, Esvg_Accumulate *accumulate);
EAPI void esvg_element_animation_repeat_count_set(Ender_Element *e, int repeat_count);
EAPI void esvg_element_animation_repeat_count_get(Ender_Element *e, int *repeat_count);

EAPI void esvg_element_animation_begin_set(Ender_Element *e, Eina_List *begin);
EAPI void esvg_element_animation_end_set(Ender_Element *e, Eina_List *end);
EAPI void esvg_element_animation_fill_set(Ender_Element *e, Esvg_Fill fill);
EAPI void esvg_element_animation_fill_get(Ender_Element *e, Esvg_Fill *fill);

#endif

