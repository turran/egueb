#ifndef _ESVG_PRIVATE_TYPES_H_
#define _ESVG_PRIVATE_TYPES_H_

/* define the attributes */
typedef struct _Esvg_Attribute_Transform
{
	Esvg_Animated_Transform v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Transform;

typedef struct _Esvg_Attribute_Length
{
	Esvg_Animated_Length v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Length,

typedef struct _Esvg_Attribute_Number
{
	Esvg_Animated_Number v;
	Eina_Bool is_set;
	int animated;
} Esvg_Attribute_Number;

#endif

