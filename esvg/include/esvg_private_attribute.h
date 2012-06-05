#ifndef _ESVG_PRIVATE_ATTRIBUTE_H_
#define _ESVG_PRIVATE_ATTRIBUTE_H_

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
} Esvg_Attribute_Animated_Number;

typedef struct _Esvg_Attribute_Animated_Color
{
	Esvg_Animated_Color v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Animated_Color;

typedef struct _Esvg_Attribute_Animated_Paint
{
	Esvg_Animated_Paint v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Animated_Paint;

typedef struct _Esvg_Attribute_Animated_String
{
	Esvg_Animated_String v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Animated_String;

typedef struct _Esvg_Attribute_Animated_Bool
{
	Esvg_Animated_Bool v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Animated_Bool;

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

#endif
