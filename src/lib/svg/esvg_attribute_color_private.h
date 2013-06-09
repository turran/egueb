#ifndef _ESVG_PRIVATE_ATTRIBUTE_COLOR_H_
#define _ESVG_PRIVATE_ATTRIBUTE_COLOR_H_

typedef struct _Egueb_Svg_Attribute_Color
{
	Egueb_Svg_Color v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_Color;

typedef struct _Egueb_Svg_Attribute_Animated_Color
{
	Egueb_Svg_Attribute_Color base;
	Egueb_Svg_Attribute_Color anim;
	int animated;
} Egueb_Svg_Attribute_Animated_Color;

void egueb_svg_attribute_animated_color_merge_rel(const Egueb_Svg_Attribute_Animated_Color *rel,
		const Egueb_Svg_Attribute_Animated_Color *v,
		Egueb_Svg_Attribute_Color *d);
void egueb_svg_attribute_animated_color_merge(const Egueb_Svg_Attribute_Animated_Color *v,
		Egueb_Svg_Attribute_Color *d);
void egueb_svg_attribute_color_merge_rel(const Egueb_Svg_Attribute_Color *rel,
		const Egueb_Svg_Attribute_Color *v,
		Egueb_Svg_Attribute_Color *d);
void egueb_svg_attribute_animated_color_set(Egueb_Svg_Attribute_Animated_Color *aa,
	const Egueb_Svg_Color_Animated *v,
	const Egueb_Svg_Color *def,
	Eina_Bool animate);
void egueb_svg_attribute_animated_color_extended_set(Egueb_Svg_Attribute_Animated_Color *aa,
	const Egueb_Svg_Color_Animated *v,
	const Egueb_Svg_Color *def,
	Eina_Bool animate,
	int *set);
void egueb_svg_attribute_animated_color_get(Egueb_Svg_Attribute_Animated_Color *aa,
	Egueb_Svg_Color_Animated *v);

void egueb_svg_attribute_color_unset(Egueb_Svg_Attribute_Color *a, const Egueb_Svg_Color *def);
void egueb_svg_attribute_color_set(Egueb_Svg_Attribute_Color *a, const Egueb_Svg_Color *v,
		const Egueb_Svg_Color *def);

#endif
