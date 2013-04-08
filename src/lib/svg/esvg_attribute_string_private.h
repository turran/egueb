#ifndef _ESVG_ATTRIBUTE_STRING_PRIVATE_H_
#define _ESVG_ATTRIBUTE_STRING_PRIVATE_H_

typedef struct _Esvg_Attribute_String
{
	char *v;
	Eina_Bool is_set;
} Esvg_Attribute_String;

typedef struct _Esvg_Attribute_Animated_String
{
	Esvg_Attribute_String base;
	Esvg_Attribute_String anim;
	int animated;
} Esvg_Attribute_Animated_String;

extern Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_string_descriptor; 

void esvg_attribute_animated_string_merge_rel(const Esvg_Attribute_Animated_String *rel,
		const Esvg_Attribute_Animated_String *v,
		Esvg_Attribute_String *d);
void esvg_attribute_animated_string_merge(const Esvg_Attribute_Animated_String *v,
		Esvg_Attribute_String *d);
void esvg_attribute_string_merge_rel(const Esvg_Attribute_String *rel,
		const Esvg_Attribute_String *v,
		Esvg_Attribute_String *d);
void esvg_attribute_animated_string_set(Esvg_Attribute_Animated_String *aa,
	const Esvg_String_Animated *v,
	Eina_Bool animate);
void esvg_attribute_animated_string_extended_set(Esvg_Attribute_Animated_String *aa,
	const Esvg_String_Animated *v,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_string_get(Esvg_Attribute_Animated_String *aa,
	Esvg_String_Animated *v);
void esvg_attribute_string_unset(Esvg_Attribute_String *a);
void esvg_attribute_string_set(Esvg_Attribute_String *a, const char *v);
void esvg_attribute_animated_string_final_get(Esvg_Attribute_Animated_String *aa, char **v);

#endif

