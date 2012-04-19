#ifndef _ESVG_ATTRIBUTE_ANIMATION_H_
#define _ESVG_ATTRIBUTE_ANIMATION_H_

#include <Eina.h>
#include <Edom.h>
#include "esvg_types.h"

typedef struct _Esvg_Attribute_Animation_Target
{
	/* attributeNAme = name
	 * attributeType = "CSS | XML | auto"
	 */
} Esvg_Attribute_Animation_Target;

typedef struct _Esvg_Attribute_Animation_Timing
{
	/* begin <begin_value_list>
	 * dur <clock> | "media" | "indefinite"
	 * end <end_value_list>
	 * min <clock> | "media"
	 * max <clock> | "media"
	 * restart "always | "whenNotActive " | "never"
	 * repeatCount = numeric | "indefinite"
	 * repeatDur = <clock> | "indefinite"
	 * fill = "freeze | remove "
	 */
} Esvg_Attribute_Animation_Timing;

typedef struct _Esvg_Attribute_Animation_Value
{
	/* calcMode = "discrete | linear | paced | spline"
	 * values = <list>
	 * keyTimes = <list>
	 * keySplines = <list>
	 * from = value
	 * to = value
	 * by = value
	 */
} Esvg_Attribute_Animation_Value;

typedef struct _Esvg_Attribute_Animation_Addition
{
	/* additive = "replace | sum "
	 * accumulate = "none | sum "
	 */
} Esvg_Attribute_Animation_Addition;

#endif
