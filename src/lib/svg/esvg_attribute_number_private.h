#ifndef _ESVG_ATTRIBUTE_NUMBER_PRIVATE_H_
#define _ESVG_ATTRIBUTE_NUMBER_PRIVATE_H_

typedef struct _Esvg_Attribute_Number
{
	Esvg_Number v;
	Eina_Bool is_set;
} Esvg_Attribute_Number;

typedef struct _Esvg_Attribute_Animated_Number
{
	Esvg_Attribute_Number base;
	Esvg_Attribute_Number anim;
	int animated;
} Esvg_Attribute_Animated_Number;

extern Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_number_descriptor; 

void esvg_attribute_animated_number_merge_rel(const Esvg_Attribute_Animated_Number *rel,
		const Esvg_Attribute_Animated_Number *v,
		Esvg_Attribute_Number *d);


void esvg_attribute_animated_number_merge(const Esvg_Attribute_Animated_Number *v,
		Esvg_Attribute_Number *d);
void esvg_attribute_number_merge_rel(const Esvg_Attribute_Number *rel,
		const Esvg_Attribute_Number *v,
		Esvg_Attribute_Number *d);

void esvg_attribute_number_unset(Esvg_Attribute_Number *a, const Esvg_Number *v);
void esvg_attribute_number_set(Esvg_Attribute_Number *a, const Esvg_Number *v,
	const Esvg_Number *def);
void esvg_attribute_animated_number_set(Esvg_Attribute_Animated_Number *aa,
	const Esvg_Number_Animated *v,
	const Esvg_Number *def,
	Eina_Bool animate);
void esvg_attribute_animated_number_extended_set(Esvg_Attribute_Animated_Number *aa,
	const Esvg_Number_Animated *v,
	const Esvg_Number *def,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_number_get(Esvg_Attribute_Animated_Number *aa,
	Esvg_Number_Animated *v);


#endif
