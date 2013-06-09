#ifndef _ESVG_ATTRIBUTE_LENGTH_PRIVATE_H_
#define _ESVG_ATTRIBUTE_LENGTH_PRIVATE_H_

typedef struct _Egueb_Svg_Attribute_Length
{
	Egueb_Svg_Length v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_Length;

typedef struct _Egueb_Svg_Attribute_Coord
{
	Egueb_Svg_Coord v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_Coord;

typedef struct _Egueb_Svg_Attribute_Animated_Length
{
	Egueb_Svg_Attribute_Length base;
	Egueb_Svg_Attribute_Length anim;
	int animated;
} Egueb_Svg_Attribute_Animated_Length;

#define Egueb_Svg_Attribute_Animated_Coord Egueb_Svg_Attribute_Animated_Length


#define ESVG_ATTRIBUTE_LENGTH(name) Egueb_Svg_Attribute_Length name
#define ESVG_ATTRIBUTE_LENGTH_FUNCTIONS(type, prefix, def, name)		\
	static void prefix##_##name##_set(type *thiz, 				\
			Egueb_Svg_Attribute_Length *name) 				\
	{									\
		Eina_Bool animating;						\
										\
		animating = egueb_svg_element_attribute_animate_get(t);		\
		egueb_svg_attribute_animated_length_set(&thiz->name, name, &def,	\
				animating);					\
	}									\
	static void prefix##_##name##_get(type *thiz, 				\
			Egueb_Svg_Attribute_Length *name) 				\
	{									\
		egueb_svg_attribute_animated_length_get(&thiz->name, name);		\
	}									\
	static Eina_Bool prefix##_##name##_is_set(type *thiz)			\
	{									\
		return egueb_svg_attribute_animated_length_is_set(thiz->name)	\
	}


void egueb_svg_attribute_animated_length_merge_rel(const Egueb_Svg_Attribute_Animated_Length *rel,
		const Egueb_Svg_Attribute_Animated_Length *v,
		Egueb_Svg_Attribute_Length *d);
void egueb_svg_attribute_animated_length_merge(const Egueb_Svg_Attribute_Animated_Length *v,
		Egueb_Svg_Attribute_Length *d);
void egueb_svg_attribute_animated_length_final_get(Egueb_Svg_Attribute_Animated_Length *aa,
	Egueb_Svg_Length *v);
void egueb_svg_attribute_length_merge_rel(const Egueb_Svg_Attribute_Length *rel,
		const Egueb_Svg_Attribute_Length *v,
		Egueb_Svg_Attribute_Length *d);
void egueb_svg_attribute_animated_length_set(Egueb_Svg_Attribute_Animated_Length *aa,
	const Egueb_Svg_Length_Animated *v,
	const Egueb_Svg_Length *def,
	Eina_Bool animate);
void egueb_svg_attribute_animated_length_extended_set(Egueb_Svg_Attribute_Animated_Length *aa,
	const Egueb_Svg_Length_Animated *v,
	const Egueb_Svg_Length *def,
	Eina_Bool animate,
	int *set);
void egueb_svg_attribute_animated_length_get(Egueb_Svg_Attribute_Animated_Length *aa,
	Egueb_Svg_Length_Animated *v);
Eina_Bool egueb_svg_attribute_animated_length_is_set(Egueb_Svg_Attribute_Animated_Length *aa);
void egueb_svg_attribute_length_unset(Egueb_Svg_Attribute_Length *a, const Egueb_Svg_Length *def);
void egueb_svg_attribute_length_set(Egueb_Svg_Attribute_Length *a, const Egueb_Svg_Length *v,
		const Egueb_Svg_Length *def);

#endif
