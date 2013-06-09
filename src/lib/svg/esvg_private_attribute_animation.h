#ifndef _ESVG_ATTRIBUTE_ANIMATION_H_
#define _ESVG_ATTRIBUTE_ANIMATION_H_

#include "egueb_svg_types.h"

typedef struct _Egueb_Svg_Attribute_Animation_Attribute_Name
{
	char *curr;
	char *prev;
} Egueb_Svg_Attribute_Animation_Attribute_Name;

typedef struct _Egueb_Svg_Attribute_Animation_Target
{
	Egueb_Svg_Attribute_Type attribute_type;
	Egueb_Svg_Attribute_Animation_Attribute_Name attribute_name;
} Egueb_Svg_Attribute_Animation_Target;

typedef struct _Egueb_Svg_Animation_Timing
{
	Eina_List *timings;
} Egueb_Svg_Animation_Timing;

typedef struct _Egueb_Svg_Attribute_Animation_Timing
{
	Eina_List *begin;
	Eina_List *end;
	Egueb_Svg_Duration dur;
	int repeat_count;
	Egueb_Svg_Fill fill;
	/* begin <begin_value_list>
	 * dur <clock> | "media" | "indefinite"
	 * end <end_value_list>
	 * min <clock> | "media"
	 * max <clock> | "media"
	 * restart "always | "whenNotActive " | "never"
	 * repeatDur = <clock> | "indefinite"
	 */
} Egueb_Svg_Attribute_Animation_Timing;

typedef struct _Egueb_Svg_Attribute_Animation_Value
{
	char *from;
	char *to;
	char *values;
	Egueb_Svg_Calc_Mode calc_mode;
	char *key_times;
	char *key_splines;
	 /* by = value
	 */
} Egueb_Svg_Attribute_Animation_Value;

typedef struct _Egueb_Svg_Attribute_Animation_Addition
{
	Egueb_Svg_Accumulate accumulate;
	Egueb_Svg_Additive additive;
} Egueb_Svg_Attribute_Animation_Addition;

typedef struct _Egueb_Svg_Attribute_Animation_Event
{
	/* onbegin */
	/* onend */
	/* onload */
	/* onrepeat */
} Egueb_Svg_Attribute_Animation_Event;

#endif
