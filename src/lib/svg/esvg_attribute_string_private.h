#ifndef _ESVG_ATTRIBUTE_STRING_PRIVATE_H_
#define _ESVG_ATTRIBUTE_STRING_PRIVATE_H_

typedef struct _Egueb_Svg_Attribute_String
{
	char *v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_String;

typedef struct _Egueb_Svg_Attribute_Animated_String
{
	Egueb_Svg_Attribute_String base;
	Egueb_Svg_Attribute_String anim;
	int animated;
} Egueb_Svg_Attribute_Animated_String;

extern Egueb_Svg_Attribute_Animated_Descriptor egueb_svg_attribute_animated_string_descriptor; 

void egueb_svg_attribute_animated_string_merge_rel(const Egueb_Svg_Attribute_Animated_String *rel,
		const Egueb_Svg_Attribute_Animated_String *v,
		Egueb_Svg_Attribute_String *d);
void egueb_svg_attribute_animated_string_merge(const Egueb_Svg_Attribute_Animated_String *v,
		Egueb_Svg_Attribute_String *d);
void egueb_svg_attribute_string_merge_rel(const Egueb_Svg_Attribute_String *rel,
		const Egueb_Svg_Attribute_String *v,
		Egueb_Svg_Attribute_String *d);
void egueb_svg_attribute_animated_string_set(Egueb_Svg_Attribute_Animated_String *aa,
	const Egueb_Svg_String_Animated *v,
	Eina_Bool animate);
void egueb_svg_attribute_animated_string_extended_set(Egueb_Svg_Attribute_Animated_String *aa,
	const Egueb_Svg_String_Animated *v,
	Eina_Bool animate,
	int *set);
void egueb_svg_attribute_animated_string_get(Egueb_Svg_Attribute_Animated_String *aa,
	Egueb_Svg_String_Animated *v);
void egueb_svg_attribute_string_unset(Egueb_Svg_Attribute_String *a);
void egueb_svg_attribute_string_set(Egueb_Svg_Attribute_String *a, const char *v);
void egueb_svg_attribute_animated_string_final_get(Egueb_Svg_Attribute_Animated_String *aa, char **v);

#endif

