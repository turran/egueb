#ifndef _ESVG_PRIVATE_ATTRIBUTE_ENUM_H_
#define _ESVG_PRIVATE_ATTRIBUTE_ENUM_H_

void esvg_attribute_animated_enum_get(Esvg_Attribute_Animated_Enum *aa,
	Esvg_Animated_Enum *v);
void * esvg_attribute_enum_value_new(void);
void * esvg_attribute_enum_destination_new(void);
void esvg_attribute_enum_destination_free(void *destination, Eina_Bool deep);
void esvg_attribute_enum_destination_value_to(void *destination, void **value);
void esvg_attribute_enum_interpolate(void *a, void *b, double m,
		void *add, void *acc, int mul, void *res);

void esvg_attribute_enum_unset(Esvg_Attribute_Enum *a, int def);
void esvg_attribute_enum_set(Esvg_Attribute_Enum *a, int v);
void esvg_attribute_animated_enum_merge_rel(const Esvg_Attribute_Animated_Enum *rel,
		const Esvg_Attribute_Animated_Enum *v,
		Esvg_Attribute_Enum *d);
void esvg_attribute_animated_enum_merge(const Esvg_Attribute_Animated_Enum *v,
		Esvg_Attribute_Enum *d);
void esvg_attribute_enum_merge_rel(const Esvg_Attribute_Enum *rel,
		const Esvg_Attribute_Enum *v,
		Esvg_Attribute_Enum *d);
void esvg_attribute_animated_enum_set(Esvg_Attribute_Animated_Enum *aa,
	const Esvg_Animated_Enum *v,
	int def,
	Eina_Bool animate);
void esvg_attribute_animated_enum_extended_set(Esvg_Attribute_Animated_Enum *aa,
	const Esvg_Animated_Enum *v,
	int def,
	Eina_Bool animate,
	int *set);

#endif

