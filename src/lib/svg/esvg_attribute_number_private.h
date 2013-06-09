#ifndef _ESVG_ATTRIBUTE_NUMBER_PRIVATE_H_
#define _ESVG_ATTRIBUTE_NUMBER_PRIVATE_H_

typedef struct _Egueb_Svg_Attribute_Number
{
	Egueb_Svg_Number v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_Number;

typedef struct _Egueb_Svg_Attribute_Animated_Number
{
	Egueb_Svg_Attribute_Number base;
	Egueb_Svg_Attribute_Number anim;
	int animated;
} Egueb_Svg_Attribute_Animated_Number;

extern Egueb_Svg_Attribute_Animated_Descriptor egueb_svg_attribute_animated_number_descriptor; 

void egueb_svg_attribute_animated_number_merge_rel(const Egueb_Svg_Attribute_Animated_Number *rel,
		const Egueb_Svg_Attribute_Animated_Number *v,
		Egueb_Svg_Attribute_Number *d);


void egueb_svg_attribute_animated_number_merge(const Egueb_Svg_Attribute_Animated_Number *v,
		Egueb_Svg_Attribute_Number *d);
void egueb_svg_attribute_number_merge_rel(const Egueb_Svg_Attribute_Number *rel,
		const Egueb_Svg_Attribute_Number *v,
		Egueb_Svg_Attribute_Number *d);

void egueb_svg_attribute_number_unset(Egueb_Svg_Attribute_Number *a, const Egueb_Svg_Number *v);
void egueb_svg_attribute_number_set(Egueb_Svg_Attribute_Number *a, const Egueb_Svg_Number *v,
	const Egueb_Svg_Number *def);
void egueb_svg_attribute_animated_number_set(Egueb_Svg_Attribute_Animated_Number *aa,
	const Egueb_Svg_Number_Animated *v,
	const Egueb_Svg_Number *def,
	Eina_Bool animate);
void egueb_svg_attribute_animated_number_extended_set(Egueb_Svg_Attribute_Animated_Number *aa,
	const Egueb_Svg_Number_Animated *v,
	const Egueb_Svg_Number *def,
	Eina_Bool animate,
	int *set);
void egueb_svg_attribute_animated_number_get(Egueb_Svg_Attribute_Animated_Number *aa,
	Egueb_Svg_Number_Animated *v);


#endif
