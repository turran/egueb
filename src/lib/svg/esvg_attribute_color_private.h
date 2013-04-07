#ifndef _ESVG_PRIVATE_ATTRIBUTE_COLOR_H_
#define _ESVG_PRIVATE_ATTRIBUTE_COLOR_H_

typedef struct _Esvg_Attribute_Color
{
	Esvg_Color v;
	Eina_Bool is_set;
} Esvg_Attribute_Color;

typedef struct _Esvg_Attribute_Animated_Color
{
	Esvg_Attribute_Color base;
	Esvg_Attribute_Color anim;
	int animated;
} Esvg_Attribute_Animated_Color;

void esvg_attribute_animated_color_merge_rel(const Esvg_Attribute_Animated_Color *rel,
		const Esvg_Attribute_Animated_Color *v,
		Esvg_Attribute_Color *d);
void esvg_attribute_animated_color_merge(const Esvg_Attribute_Animated_Color *v,
		Esvg_Attribute_Color *d);
void esvg_attribute_color_merge_rel(const Esvg_Attribute_Color *rel,
		const Esvg_Attribute_Color *v,
		Esvg_Attribute_Color *d);
void esvg_attribute_animated_color_set(Esvg_Attribute_Animated_Color *aa,
	const Esvg_Color_Animated *v,
	const Esvg_Color *def,
	Eina_Bool animate);
void esvg_attribute_animated_color_extended_set(Esvg_Attribute_Animated_Color *aa,
	const Esvg_Color_Animated *v,
	const Esvg_Color *def,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_color_get(Esvg_Attribute_Animated_Color *aa,
	Esvg_Color_Animated *v);

void esvg_attribute_color_unset(Esvg_Attribute_Color *a, const Esvg_Color *def);
void esvg_attribute_color_set(Esvg_Attribute_Color *a, const Esvg_Color *v,
		const Esvg_Color *def);

#endif
