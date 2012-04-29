#ifndef _ESVG_ANIMATION_H_
#define _ESVG_ANIMATION_H_

EAPI void esvg_animation_attribute_name_set(Ender_Element *e, const char *name);
EAPI void esvg_animation_attribute_name_get(Ender_Element *e, const char **name);
EAPI void esvg_animation_attribute_type_set(Ender_Element *e, Esvg_Attribute_Type type);
EAPI void esvg_animation_attribute_type_get(Ender_Element *e, Esvg_Attribute_Type *type);

#endif

