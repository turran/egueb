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

typedef struct _Esvg_Attribute_List
{
	Eina_List *v;
	Eina_Bool is_set;
} Esvg_Attribute_List;

/* animated */
typedef void * (*Esvg_Attribute_Animated_Value_New)(void);
typedef Eina_Bool (*Esvg_Attribute_Animated_Value_Get)(const char *attr, void **value);
typedef void (*Esvg_Attribute_Animated_Value_Free)(void *value);
typedef void * (*Esvg_Attribute_Animated_Destination_New)(void);
/* create the destination attribute struct holder */
typedef void (*Esvg_Attribute_Animated_Destination_Keep)(void *destination);
/* free the destination attribute struct holder, in case deep is set, also free the internal base/anim values */
typedef void (*Esvg_Attribute_Animated_Destination_Free)(void *destination, Eina_Bool deep);
typedef void (*Esvg_Attribute_Animated_Destination_Value_From)(void *destination, void *value);
typedef void (*Esvg_Attribute_Animated_Destination_Value_To)(void *destination, void **value);
/* later add the accumulator, additive, etc */
typedef void (*Esvg_Animate_Base_Interpolator)(void *a, void *b, double m,
		void *add, void *acc, int mul, void *res);

typedef struct _Esvg_Attribute_Animated_Descriptor
{
	/* to generate the values */
	Esvg_Attribute_Animated_Value_New value_new;
	Esvg_Attribute_Animated_Value_Get value_get;
	Esvg_Attribute_Animated_Value_Free value_free;
	/* to generate the destination value */
	Esvg_Attribute_Animated_Destination_New destination_new;
	Esvg_Attribute_Animated_Destination_Free destination_free;
	Esvg_Attribute_Animated_Destination_Keep destination_keep;
	Esvg_Attribute_Animated_Destination_Value_From destination_value_from;
	Esvg_Attribute_Animated_Destination_Value_To destination_value_to;
	/* the interpolator */
	Esvg_Animate_Base_Interpolator interpolator;
} Esvg_Attribute_Animated_Descriptor;

extern Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_paint_descriptor; 
extern Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_string_descriptor; 
extern Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_path_command_descriptor; 
extern Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_length_descriptor; 
extern Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_number_descriptor; 
Esvg_Attribute_Animated_Descriptor * esvg_attribute_animated_descriptor_get(const char *name);

typedef struct _Esvg_Attribute_Animated_Length
{
	Esvg_Attribute_Length base;
	Esvg_Attribute_Length anim;
	int animated;
} Esvg_Attribute_Animated_Length;

#define Esvg_Attribute_Animated_Coord Esvg_Attribute_Animated_Length

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

typedef struct _Esvg_Attribute_Animated_List
{
	Esvg_Attribute_List base;
	Esvg_Attribute_List anim;
	int animated;
} Esvg_Attribute_Animated_List;

typedef struct _Esvg_Attribute_Animated_Transform
{
	Esvg_Attribute_Transform base;
	Esvg_Attribute_Transform anim;
	int animated;
} Esvg_Attribute_Animated_Transform;

void esvg_attribute_animated_list_add(Esvg_Attribute_Animated_List *aa,
	void *data,
	Eina_Bool animate);
void esvg_attribute_animated_list_final_get(Esvg_Attribute_Animated_List *aa, Eina_List **v);
void esvg_attribute_animated_list_get(Esvg_Attribute_Animated_List *aa,
	Esvg_Animated_List *v);

void esvg_attribute_animated_color_merge_rel(const Esvg_Attribute_Animated_Color *rel,
		const Esvg_Attribute_Animated_Color *v,
		Esvg_Attribute_Color *d);
void esvg_attribute_animated_color_merge(const Esvg_Attribute_Animated_Color *v,
		Esvg_Attribute_Color *d);
void esvg_attribute_color_merge_rel(const Esvg_Attribute_Color *rel,
		const Esvg_Attribute_Color *v,
		Esvg_Attribute_Color *d);
void esvg_attribute_animated_color_set(Esvg_Attribute_Animated_Color *aa,
	const Esvg_Animated_Color *v,
	const Esvg_Color *def,
	Eina_Bool animate);
void esvg_attribute_animated_color_extended_set(Esvg_Attribute_Animated_Color *aa,
	const Esvg_Animated_Color *v,
	const Esvg_Color *def,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_color_get(Esvg_Attribute_Animated_Color *aa,
	Esvg_Animated_Color *v);


void esvg_attribute_animated_string_merge_rel(const Esvg_Attribute_Animated_String *rel,
		const Esvg_Attribute_Animated_String *v,
		Esvg_Attribute_String *d);
void esvg_attribute_animated_string_merge(const Esvg_Attribute_Animated_String *v,
		Esvg_Attribute_String *d);
void esvg_attribute_string_merge_rel(const Esvg_Attribute_String *rel,
		const Esvg_Attribute_String *v,
		Esvg_Attribute_String *d);
void esvg_attribute_animated_string_set(Esvg_Attribute_Animated_String *aa,
	const Esvg_Animated_String *v,
	Eina_Bool animate);
void esvg_attribute_animated_string_extended_set(Esvg_Attribute_Animated_String *aa,
	const Esvg_Animated_String *v,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_string_get(Esvg_Attribute_Animated_String *aa,
	Esvg_Animated_String *v);
void esvg_attribute_string_unset(Esvg_Attribute_String *a);
void esvg_attribute_string_set(Esvg_Attribute_String *a, const char *v);
void esvg_attribute_animated_string_final_get(Esvg_Attribute_Animated_String *aa, char **v);

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


void esvg_attribute_animated_bool_merge_rel(const Esvg_Attribute_Animated_Bool *rel,
		const Esvg_Attribute_Animated_Bool *v,
		Esvg_Attribute_Bool *d);
void esvg_attribute_animated_bool_merge(const Esvg_Attribute_Animated_Bool *v,
		Esvg_Attribute_Bool *d);
void esvg_attribute_bool_merge_rel(const Esvg_Attribute_Bool *rel,
		const Esvg_Attribute_Bool *v,
		Esvg_Attribute_Bool *d);
void esvg_attribute_animated_bool_set(Esvg_Attribute_Animated_Bool *aa,
	const Esvg_Animated_Bool *v,
	Eina_Bool def,
	Eina_Bool animate);
void esvg_attribute_animated_bool_extended_set(Esvg_Attribute_Animated_Bool *aa,
	const Esvg_Animated_Bool *v,
	Eina_Bool def,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_bool_get(Esvg_Attribute_Animated_Bool *aa,
	Esvg_Animated_Bool *v);
void esvg_attribute_bool_unset(Esvg_Attribute_Bool *a, Eina_Bool def);
void esvg_attribute_bool_set(Esvg_Attribute_Bool *a, Eina_Bool v);void esvg_attribute_animated_number_merge_rel(const Esvg_Attribute_Animated_Number *rel,
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

void esvg_attribute_transform_set(Esvg_Attribute_Transform *a, const Enesim_Matrix *v, const Enesim_Matrix *def);
void esvg_attribute_transform_unset(Esvg_Attribute_Transform *a, const Enesim_Matrix *def);
void esvg_attribute_animated_transform_set(Esvg_Attribute_Animated_Transform *aa,
	const Esvg_Animated_Transform *v,
	const Enesim_Matrix *def,
	Eina_Bool animate);
void esvg_attribute_animated_transform_extended_set(Esvg_Attribute_Animated_Transform *aa,
	const Esvg_Animated_Transform *v,
	const Enesim_Matrix *def,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_transform_get(Esvg_Attribute_Animated_Transform *aa,
	Esvg_Animated_Transform *v);
void esvg_attribute_animated_transform_final_get(Esvg_Attribute_Animated_Transform *aa, Enesim_Matrix *m);
Eina_Bool esvg_attribute_animated_transform_is_set(Esvg_Attribute_Animated_Transform *aa);


void esvg_attribute_color_unset(Esvg_Attribute_Color *a, const Esvg_Color *def);
void esvg_attribute_color_set(Esvg_Attribute_Color *a, const Esvg_Color *v,
		const Esvg_Color *def);
void esvg_attribute_paint_unset(Esvg_Attribute_Paint *a, const Esvg_Paint *def);


void esvg_attribute_paint_set(Esvg_Attribute_Paint *a, const Esvg_Paint *v,
		const Esvg_Paint *def);
void esvg_attribute_animated_paint_set(Esvg_Attribute_Animated_Paint *aa,
	const Esvg_Paint_Animated *v,
	const Esvg_Paint *def,
	Eina_Bool animate);
void esvg_attribute_animated_paint_extended_set(Esvg_Attribute_Animated_Paint *aa,
	const Esvg_Paint_Animated *v,
	const Esvg_Paint *def,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_paint_get(Esvg_Attribute_Animated_Paint *aa,
	Esvg_Paint_Animated *v);


void esvg_attribute_number_unset(Esvg_Attribute_Number *a, double def);
void esvg_attribute_number_set(Esvg_Attribute_Number *a, double v);
void esvg_attribute_animated_number_set(Esvg_Attribute_Animated_Number *aa,
	const Esvg_Animated_Number *v,
	double def,
	Eina_Bool animate);
void esvg_attribute_animated_number_extended_set(Esvg_Attribute_Animated_Number *aa,
	const Esvg_Animated_Number *v,
	double def,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_number_get(Esvg_Attribute_Animated_Number *aa,
	Esvg_Animated_Number *v);


#include "esvg_private_attribute_display.h"
#include "esvg_private_attribute_clip_path.h"
#include "esvg_private_attribute_enum.h"
#include "esvg_private_attribute_visibility.h"

#endif
