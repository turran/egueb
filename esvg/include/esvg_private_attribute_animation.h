#ifndef _ESVG_ATTRIBUTE_ANIMATION_H_
#define _ESVG_ATTRIBUTE_ANIMATION_H_

#include <Eina.h>
#include <Edom.h>
#include "esvg_types.h"

typedef struct _Esvg_Attribute_Animation_Attribute_Name
{
	char *curr;
	char *prev;
	Eina_Bool changed : 1;
} Esvg_Attribute_Animation_Attribute_Name;

typedef struct _Esvg_Attribute_Animation_Target
{
	Esvg_Attribute_Type attribute_type;
	Esvg_Attribute_Animation_Attribute_Name attribute_name;
} Esvg_Attribute_Animation_Target;

typedef struct _Esvg_Animation_Timing
{
	Eina_List *timings;
} Esvg_Animation_Timing;

typedef struct _Esvg_Attribute_Animation_Timing
{
	Eina_List *begin;
	Eina_List *end;
	Esvg_Duration dur;
	int repeat_count;
	/* begin <begin_value_list>
	 * dur <clock> | "media" | "indefinite"
	 * end <end_value_list>
	 * min <clock> | "media"
	 * max <clock> | "media"
	 * restart "always | "whenNotActive " | "never"
	 * repeatDur = <clock> | "indefinite"
	 * fill = "freeze | remove "
	 */
} Esvg_Attribute_Animation_Timing;

typedef struct _Esvg_Attribute_Animation_Value
{
	char *from;
	char *to;
	char *values;
	Esvg_Calc_Mode calc_mode;
	char *key_times;
	char *key_splines;
	 /* by = value
	 */
} Esvg_Attribute_Animation_Value;

typedef struct _Esvg_Attribute_Animation_Addition
{
	Esvg_Accumulate accumulate;
	Esvg_Additive additive;
} Esvg_Attribute_Animation_Addition;

typedef struct _Esvg_Attribute_Animation_Event
{
	/* onbegin */
	/* onend */
	/* onload */
	/* onrepeat */
} Esvg_Attribute_Animation_Event;

#endif
