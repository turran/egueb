#ifndef _ESVG_ATTRIBUTE_LENGTH_PRIVATE_H_
#define _ESVG_ATTRIBUTE_LENGTH_PRIVATE_H_

typedef struct _Esvg_Attribute_Length
{
	Esvg_Length v;
	Eina_Bool is_set;
} Esvg_Attribute_Length;

typedef struct _Esvg_Attribute_Coord
{
	Esvg_Coord v;
	Eina_Bool is_set;
} Esvg_Attribute_Coord;

typedef struct _Esvg_Attribute_Animated_Length
{
	Esvg_Attribute_Length base;
	Esvg_Attribute_Length anim;
	int animated;
} Esvg_Attribute_Animated_Length;

#define Esvg_Attribute_Animated_Coord Esvg_Attribute_Animated_Length


#define ESVG_ATTRIBUTE_LENGTH(name) Esvg_Attribute_Length name
#define ESVG_ATTRIBUTE_LENGTH_FUNCTIONS(type, prefix, def, name)		\
	static void prefix##_##name##_set(type *thiz, 				\
			Esvg_Attribute_Length *name) 				\
	{									\
		Eina_Bool animating;						\
										\
		animating = esvg_element_attribute_animate_get(t);		\
		esvg_attribute_animated_length_set(&thiz->name, name, &def,	\
				animating);					\
	}									\
	static void prefix##_##name##_get(type *thiz, 				\
			Esvg_Attribute_Length *name) 				\
	{									\
		esvg_attribute_animated_length_get(&thiz->name, name);		\
	}									\
	static Eina_Bool prefix##_##name##_is_set(type *thiz)			\
	{									\
		return esvg_attribute_animated_length_is_set(thiz->name)	\
	}


void esvg_attribute_animated_length_merge_rel(const Esvg_Attribute_Animated_Length *rel,
		const Esvg_Attribute_Animated_Length *v,
		Esvg_Attribute_Length *d);
void esvg_attribute_animated_length_merge(const Esvg_Attribute_Animated_Length *v,
		Esvg_Attribute_Length *d);
void esvg_attribute_animated_length_final_get(Esvg_Attribute_Animated_Length *aa,
	Esvg_Length *v);
void esvg_attribute_length_merge_rel(const Esvg_Attribute_Length *rel,
		const Esvg_Attribute_Length *v,
		Esvg_Attribute_Length *d);
void esvg_attribute_animated_length_set(Esvg_Attribute_Animated_Length *aa,
	const Esvg_Length_Animated *v,
	const Esvg_Length *def,
	Eina_Bool animate);
void esvg_attribute_animated_length_extended_set(Esvg_Attribute_Animated_Length *aa,
	const Esvg_Length_Animated *v,
	const Esvg_Length *def,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_length_get(Esvg_Attribute_Animated_Length *aa,
	Esvg_Length_Animated *v);
Eina_Bool esvg_attribute_animated_length_is_set(Esvg_Attribute_Animated_Length *aa);
void esvg_attribute_length_unset(Esvg_Attribute_Length *a, const Esvg_Length *def);
void esvg_attribute_length_set(Esvg_Attribute_Length *a, const Esvg_Length *v,
		const Esvg_Length *def);

#endif
