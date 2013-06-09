#ifndef _ESVG_PRIVATE_ATTRIBUTE_H_
#define _ESVG_PRIVATE_ATTRIBUTE_H_

/* non animated */

typedef struct _Egueb_Svg_Attribute_Bool
{
	Eina_Bool v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_Bool;

typedef struct _Egueb_Svg_Attribute_Paint
{
	Egueb_Svg_Paint v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_Paint;

typedef struct _Egueb_Svg_Attribute_Transform
{
	Enesim_Matrix v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_Transform;

typedef struct _Egueb_Svg_Attribute_Enum
{
	int v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_Enum;

typedef struct _Egueb_Svg_Attribute_List
{
	Eina_List *v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_List;

/* animated */
typedef void * (*Egueb_Svg_Attribute_Animated_Value_New)(void);
typedef Eina_Bool (*Egueb_Svg_Attribute_Animated_Value_Get)(const char *attr, void **value);
typedef void (*Egueb_Svg_Attribute_Animated_Value_Free)(void *value);
typedef void * (*Egueb_Svg_Attribute_Animated_Destination_New)(void);
/* create the destination attribute struct holder */
typedef void (*Egueb_Svg_Attribute_Animated_Destination_Keep)(void *destination);
/* free the destination attribute struct holder, in case deep is set, also free the internal base/anim values */
typedef void (*Egueb_Svg_Attribute_Animated_Destination_Free)(void *destination, Eina_Bool deep);
typedef void (*Egueb_Svg_Attribute_Animated_Destination_Value_From)(void *destination, void *value);
typedef void (*Egueb_Svg_Attribute_Animated_Destination_Value_To)(void *destination, void **value);
/* later add the accumulator, additive, etc */
typedef void (*Egueb_Svg_Animate_Base_Interpolator)(void *a, void *b, double m,
		void *add, void *acc, int mul, void *res);

typedef struct _Egueb_Svg_Attribute_Animated_Descriptor
{
	/* to generate the values */
	Egueb_Svg_Attribute_Animated_Value_New value_new;
	Egueb_Svg_Attribute_Animated_Value_Get value_get;
	Egueb_Svg_Attribute_Animated_Value_Free value_free;
	/* to generate the destination value */
	Egueb_Svg_Attribute_Animated_Destination_New destination_new;
	Egueb_Svg_Attribute_Animated_Destination_Free destination_free;
	Egueb_Svg_Attribute_Animated_Destination_Keep destination_keep;
	Egueb_Svg_Attribute_Animated_Destination_Value_From destination_value_from;
	Egueb_Svg_Attribute_Animated_Destination_Value_To destination_value_to;
	/* the interpolator */
	Egueb_Svg_Animate_Base_Interpolator interpolator;
} Egueb_Svg_Attribute_Animated_Descriptor;

extern Egueb_Svg_Attribute_Animated_Descriptor egueb_svg_attribute_animated_paint_descriptor; 
extern Egueb_Svg_Attribute_Animated_Descriptor egueb_svg_attribute_animated_path_command_descriptor; 
extern Egueb_Svg_Attribute_Animated_Descriptor egueb_svg_attribute_animated_length_descriptor; 
Egueb_Svg_Attribute_Animated_Descriptor * egueb_svg_attribute_animated_descriptor_get(const char *name);

typedef struct _Egueb_Svg_Attribute_Animated_Paint
{
	Egueb_Svg_Attribute_Paint base;
	Egueb_Svg_Attribute_Paint anim;
	int animated;
} Egueb_Svg_Attribute_Animated_Paint;

typedef struct _Egueb_Svg_Attribute_Animated_Bool
{
	Egueb_Svg_Attribute_Bool base;
	Egueb_Svg_Attribute_Bool anim;
	int animated;
} Egueb_Svg_Attribute_Animated_Bool;

typedef struct _Egueb_Svg_Attribute_Animated_Enum
{
	Egueb_Svg_Attribute_Enum base;
	Egueb_Svg_Attribute_Enum anim;
	int animated;
} Egueb_Svg_Attribute_Animated_Enum;

typedef struct _Egueb_Svg_Attribute_Animated_List
{
	Egueb_Svg_Attribute_List base;
	Egueb_Svg_Attribute_List anim;
	int animated;
} Egueb_Svg_Attribute_Animated_List;

typedef struct _Egueb_Svg_Attribute_Animated_Transform
{
	Egueb_Svg_Attribute_Transform base;
	Egueb_Svg_Attribute_Transform anim;
	int animated;
} Egueb_Svg_Attribute_Animated_Transform;

void egueb_svg_attribute_animated_list_add(Egueb_Svg_Attribute_Animated_List *aa,
	void *data,
	Eina_Bool animate);
void egueb_svg_attribute_animated_list_final_get(Egueb_Svg_Attribute_Animated_List *aa, Eina_List **v);
void egueb_svg_attribute_animated_list_get(Egueb_Svg_Attribute_Animated_List *aa,
	Egueb_Svg_Animated_List *v);

void egueb_svg_attribute_animated_bool_merge_rel(const Egueb_Svg_Attribute_Animated_Bool *rel,
		const Egueb_Svg_Attribute_Animated_Bool *v,
		Egueb_Svg_Attribute_Bool *d);
void egueb_svg_attribute_animated_bool_merge(const Egueb_Svg_Attribute_Animated_Bool *v,
		Egueb_Svg_Attribute_Bool *d);
void egueb_svg_attribute_bool_merge_rel(const Egueb_Svg_Attribute_Bool *rel,
		const Egueb_Svg_Attribute_Bool *v,
		Egueb_Svg_Attribute_Bool *d);
void egueb_svg_attribute_animated_bool_set(Egueb_Svg_Attribute_Animated_Bool *aa,
	const Egueb_Svg_Boolean_Animated *v,
	Eina_Bool def,
	Eina_Bool animate);
void egueb_svg_attribute_animated_bool_extended_set(Egueb_Svg_Attribute_Animated_Bool *aa,
	const Egueb_Svg_Boolean_Animated *v,
	Eina_Bool def,
	Eina_Bool animate,
	int *set);
void egueb_svg_attribute_animated_bool_get(Egueb_Svg_Attribute_Animated_Bool *aa,
	Egueb_Svg_Boolean_Animated *v);
void egueb_svg_attribute_bool_unset(Egueb_Svg_Attribute_Bool *a, Eina_Bool def);
void egueb_svg_attribute_bool_set(Egueb_Svg_Attribute_Bool *a, Eina_Bool v);
void egueb_svg_attribute_animated_paint_merge_rel(const Egueb_Svg_Attribute_Animated_Paint *rel,
		const Egueb_Svg_Attribute_Animated_Paint *v,
		Egueb_Svg_Attribute_Paint *d);
void egueb_svg_attribute_animated_paint_merge(const Egueb_Svg_Attribute_Animated_Paint *v,
		Egueb_Svg_Attribute_Paint *d);
void egueb_svg_attribute_paint_merge_rel(const Egueb_Svg_Attribute_Paint *rel,
		const Egueb_Svg_Attribute_Paint *v,
		Egueb_Svg_Attribute_Paint *d);

void egueb_svg_attribute_transform_set(Egueb_Svg_Attribute_Transform *a, const Enesim_Matrix *v, const Enesim_Matrix *def);
void egueb_svg_attribute_transform_unset(Egueb_Svg_Attribute_Transform *a, const Enesim_Matrix *def);
void egueb_svg_attribute_animated_transform_set(Egueb_Svg_Attribute_Animated_Transform *aa,
	const Egueb_Svg_Matrix_Animated *v,
	const Enesim_Matrix *def,
	Eina_Bool animate);
void egueb_svg_attribute_animated_transform_extended_set(Egueb_Svg_Attribute_Animated_Transform *aa,
	const Egueb_Svg_Matrix_Animated *v,
	const Enesim_Matrix *def,
	Eina_Bool animate,
	int *set);
void egueb_svg_attribute_animated_transform_get(Egueb_Svg_Attribute_Animated_Transform *aa,
	Egueb_Svg_Matrix_Animated *v);
void egueb_svg_attribute_animated_transform_final_get(Egueb_Svg_Attribute_Animated_Transform *aa, Enesim_Matrix *m);
Eina_Bool egueb_svg_attribute_animated_transform_is_set(Egueb_Svg_Attribute_Animated_Transform *aa);


void egueb_svg_attribute_paint_unset(Egueb_Svg_Attribute_Paint *a, const Egueb_Svg_Paint *def);


void egueb_svg_attribute_paint_set(Egueb_Svg_Attribute_Paint *a, const Egueb_Svg_Paint *v,
		const Egueb_Svg_Paint *def);
void egueb_svg_attribute_animated_paint_set(Egueb_Svg_Attribute_Animated_Paint *aa,
	const Egueb_Svg_Paint_Animated *v,
	const Egueb_Svg_Paint *def,
	Eina_Bool animate);
void egueb_svg_attribute_animated_paint_extended_set(Egueb_Svg_Attribute_Animated_Paint *aa,
	const Egueb_Svg_Paint_Animated *v,
	const Egueb_Svg_Paint *def,
	Eina_Bool animate,
	int *set);
void egueb_svg_attribute_animated_paint_get(Egueb_Svg_Attribute_Animated_Paint *aa,
	Egueb_Svg_Paint_Animated *v);

#include "egueb_svg_attribute_display_private.h"
#include "egueb_svg_attribute_clip_path_private.h"
#include "egueb_svg_attribute_color_private.h"
#include "egueb_svg_attribute_enum_private.h"
#include "egueb_svg_attribute_length_private.h"
#include "egueb_svg_attribute_number_private.h"
#include "egueb_svg_attribute_string_private.h"
#include "egueb_svg_attribute_visibility_private.h"

#endif
