#ifndef _ESVG_PRIVATE_ATTRIBUTE_ENUM_H_
#define _ESVG_PRIVATE_ATTRIBUTE_ENUM_H_

void egueb_svg_attribute_animated_enum_get(Egueb_Svg_Attribute_Animated_Enum *aa,
	Egueb_Svg_Animated_Enum *v);
void * egueb_svg_attribute_enum_value_new(void);
void * egueb_svg_attribute_enum_destination_new(void);
void egueb_svg_attribute_enum_destination_free(void *destination, Eina_Bool deep);
void egueb_svg_attribute_enum_destination_value_to(void *destination, void **value);
void egueb_svg_attribute_enum_interpolate(void *a, void *b, double m,
		void *add, void *acc, int mul, void *res);

void egueb_svg_attribute_enum_unset(Egueb_Svg_Attribute_Enum *a, int def);
void egueb_svg_attribute_enum_set(Egueb_Svg_Attribute_Enum *a, int v);
void egueb_svg_attribute_animated_enum_merge_rel(const Egueb_Svg_Attribute_Animated_Enum *rel,
		const Egueb_Svg_Attribute_Animated_Enum *v,
		Egueb_Svg_Attribute_Enum *d);
void egueb_svg_attribute_animated_enum_merge(const Egueb_Svg_Attribute_Animated_Enum *v,
		Egueb_Svg_Attribute_Enum *d);
void egueb_svg_attribute_enum_merge_rel(const Egueb_Svg_Attribute_Enum *rel,
		const Egueb_Svg_Attribute_Enum *v,
		Egueb_Svg_Attribute_Enum *d);
void egueb_svg_attribute_animated_enum_set(Egueb_Svg_Attribute_Animated_Enum *aa,
	const Egueb_Svg_Animated_Enum *v,
	int def,
	Eina_Bool animate);
void egueb_svg_attribute_animated_enum_extended_set(Egueb_Svg_Attribute_Animated_Enum *aa,
	const Egueb_Svg_Animated_Enum *v,
	int def,
	Eina_Bool animate,
	int *set);

#endif

