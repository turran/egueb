#ifndef _ESVG_PRIVATE_TYPES_H_
#define _ESVG_PRIVATE_TYPES_H_

/* FIXME this should go into its own
 * header esvg_private_attribute.h
 */

/* define the attributes */
typedef struct _Esvg_Attribute_Animated_Transform
{
	Esvg_Animated_Transform v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Animated_Transform;

typedef struct _Esvg_Attribute_Animated_Length
{
	Esvg_Animated_Length v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Animated_Length;

typedef struct _Esvg_Attribute_Animated_Number
{
	Esvg_Animated_Number v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Number;

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

void esvg_paint_copy(Esvg_Paint *dst, Esvg_Paint *src);

#endif

