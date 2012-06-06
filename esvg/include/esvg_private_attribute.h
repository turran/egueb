#ifndef _ESVG_PRIVATE_ATTRIBUTE_H_
#define _ESVG_PRIVATE_ATTRIBUTE_H_

/* non animated */

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

typedef struct _Esvg_Attribute_Bool
{
	Eina_Bool v;
	Eina_Bool is_set;
} Esvg_Attribute_Bool;

typedef struct _Esvg_Attribute_Color
{
	Esvg_Color v;
	Eina_Bool is_set;
} Esvg_Attribute_Color;

typedef struct _Esvg_Attribute_String
{
	char *v;
	Eina_Bool is_set;
} Esvg_Attribute_String;

typedef struct _Esvg_Attribute_Paint
{
	Esvg_Paint v;
	Eina_Bool is_set;
} Esvg_Attribute_Paint;

typedef struct _Esvg_Attribute_Transform
{
	Enesim_Matrix v;
	Eina_Bool is_set;
} Esvg_Attribute_Transform;

typedef struct _Esvg_Attribute_Number
{
	double v;
	Eina_Bool is_set;
} Esvg_Attribute_Number;

typedef struct _Esvg_Attribute_Enum
{
	int v;
	Eina_Bool is_set;
} Esvg_Attribute_Enum;

/* animated */
typedef struct _Esvg_Attribute_Animated_Transform
{
	Esvg_Attribute_Transform base;
	Esvg_Attribute_Transform anim;
	int animated;
} Esvg_Attribute_Animated_Transform;

typedef struct _Esvg_Attribute_Animated_Length
{
	Esvg_Attribute_Length base;
	Esvg_Attribute_Length anim;
	int animated;
} Esvg_Attribute_Animated_Length;

typedef struct _Esvg_Attribute_Animated_Number
{
	Esvg_Attribute_Number base;
	Esvg_Attribute_Number anim;
	int animated;
} Esvg_Attribute_Animated_Number;

typedef struct _Esvg_Attribute_Animated_Color
{
	Esvg_Attribute_Color base;
	Esvg_Attribute_Color anim;
	int animated;
} Esvg_Attribute_Animated_Color;

typedef struct _Esvg_Attribute_Animated_Paint
{
	Esvg_Attribute_Paint base;
	Esvg_Attribute_Paint anim;
	int animated;
} Esvg_Attribute_Animated_Paint;

typedef struct _Esvg_Attribute_Animated_String
{
	Esvg_Attribute_String base;
	Esvg_Attribute_String anim;
	int animated;
} Esvg_Attribute_Animated_String;

typedef struct _Esvg_Attribute_Animated_Bool
{
	Esvg_Attribute_Bool base;
	Esvg_Attribute_Bool anim;
	int animated;
} Esvg_Attribute_Animated_Bool;

typedef struct _Esvg_Attribute_Animated_Enum
{
	Esvg_Attribute_Enum base;
	Esvg_Attribute_Enum anim;
	int animated;
} Esvg_Attribute_Animated_Enum;

void esvg_attribute_animated_color_merge_rel(const Esvg_Attribute_Animated_Color *rel,
		const Esvg_Attribute_Animated_Color *v,
		Esvg_Attribute_Color *d);
void esvg_attribute_animated_color_merge(const Esvg_Attribute_Animated_Color *v,
		Esvg_Attribute_Color *d);
void esvg_attribute_color_merge_rel(const Esvg_Attribute_Color *rel,
		const Esvg_Attribute_Color *v,
		Esvg_Attribute_Color *d);
void esvg_attribute_animated_string_merge_rel(const Esvg_Attribute_Animated_String *rel,
		const Esvg_Attribute_Animated_String *v,
		Esvg_Attribute_String *d);
void esvg_attribute_animated_string_merge(const Esvg_Attribute_Animated_String *v,
		Esvg_Attribute_String *d);
void esvg_attribute_string_merge_rel(const Esvg_Attribute_String *rel,
		const Esvg_Attribute_String *v,
		Esvg_Attribute_String *d);
void esvg_attribute_animated_length_merge_rel(const Esvg_Attribute_Animated_Length *rel,
		const Esvg_Attribute_Animated_Length *v,
		Esvg_Attribute_Length *d);
void esvg_attribute_animated_length_merge(const Esvg_Attribute_Animated_Length *v,
		Esvg_Attribute_Length *d);
void esvg_attribute_length_merge_rel(const Esvg_Attribute_Length *rel,
		const Esvg_Attribute_Length *v,
		Esvg_Attribute_Length *d);
void esvg_attribute_animated_bool_merge_rel(const Esvg_Attribute_Animated_Bool *rel,
		const Esvg_Attribute_Animated_Bool *v,
		Esvg_Attribute_Bool *d);
void esvg_attribute_animated_bool_merge(const Esvg_Attribute_Animated_Bool *v,
		Esvg_Attribute_Bool *d);
void esvg_attribute_bool_merge_rel(const Esvg_Attribute_Bool *rel,
		const Esvg_Attribute_Bool *v,
		Esvg_Attribute_Bool *d);
void esvg_attribute_animated_number_merge_rel(const Esvg_Attribute_Animated_Number *rel,
		const Esvg_Attribute_Animated_Number *v,
		Esvg_Attribute_Number *d);
void esvg_attribute_animated_number_merge(const Esvg_Attribute_Animated_Number *v,
		Esvg_Attribute_Number *d);
void esvg_attribute_number_merge_rel(const Esvg_Attribute_Number *rel,
		const Esvg_Attribute_Number *v,
		Esvg_Attribute_Number *d);
void esvg_attribute_animated_paint_merge_rel(const Esvg_Attribute_Animated_Paint *rel,
		const Esvg_Attribute_Animated_Paint *v,
		Esvg_Attribute_Paint *d);
void esvg_attribute_animated_paint_merge(const Esvg_Attribute_Animated_Paint *v,
		Esvg_Attribute_Paint *d);
void esvg_attribute_paint_merge_rel(const Esvg_Attribute_Paint *rel,
		const Esvg_Attribute_Paint *v,
		Esvg_Attribute_Paint *d);
void esvg_attribute_animated_enum_merge_rel(const Esvg_Attribute_Animated_Enum *rel,
		const Esvg_Attribute_Animated_Enum *v,
		Esvg_Attribute_Enum *d);
void esvg_attribute_animated_enum_merge(const Esvg_Attribute_Animated_Enum *v,
		Esvg_Attribute_Enum *d);
void esvg_attribute_enum_merge_rel(const Esvg_Attribute_Enum *rel,
		const Esvg_Attribute_Enum *v,
		Esvg_Attribute_Enum *d);
void esvg_attribute_string_unset(Esvg_Attribute_String *a);
void esvg_attribute_string_set(Esvg_Attribute_String *a, const char *v);
void esvg_attribute_color_unset(Esvg_Attribute_Color *a, const Esvg_Color *def);
void esvg_attribute_color_set(Esvg_Attribute_Color *a, const Esvg_Color *v,
		const Esvg_Color *def);
void esvg_attribute_paint_unset(Esvg_Attribute_Paint *a, const Esvg_Paint *def);
void esvg_attribute_paint_set(Esvg_Attribute_Paint *a, const Esvg_Paint *v,
		const Esvg_Paint *def);
void esvg_attribute_number_unset(Esvg_Attribute_Number *a, double def);
void esvg_attribute_number_set(Esvg_Attribute_Number *a, double v);
void esvg_attribute_length_unset(Esvg_Attribute_Length *a, const Esvg_Length *def);
void esvg_attribute_length_set(Esvg_Attribute_Length *a, const Esvg_Length *v,
		const Esvg_Length *def);
void esvg_attribute_bool_unset(Esvg_Attribute_Bool *a, Eina_Bool def);
void esvg_attribute_bool_set(Esvg_Attribute_Bool *a, Eina_Bool v);
void esvg_attribute_enum_unset(Esvg_Attribute_Enum *a, int def);
void esvg_attribute_enum_set(Esvg_Attribute_Enum *a, int v);

#endif
