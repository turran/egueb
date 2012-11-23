#ifndef _ESVG_PRIVATE_ANIMATE_BASE_H_
#define _ESVG_PRIVATE_ANIMATE_BASE_H_

#include "esvg_private_attribute_animation.h"

typedef struct _Esvg_Animate_Base_Context {
	Esvg_Attribute_Animation_Value value;
	Eina_Bool changed : 1;
} Esvg_Animate_Base_Context;

typedef Eina_Bool (*Esvg_Animate_Base_Attribute_Descriptor_Get)(Edom_Tag *t,
		const char *name,
		Esvg_Attribute_Animated_Descriptor **d);

typedef struct _Esvg_Animate_Base_Descriptor {
	/* the tag interface */
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	/* the animate_base interface */
	Esvg_Animate_Base_Attribute_Descriptor_Get type_descriptor_get;
} Esvg_Animate_Base_Descriptor;

Eina_Bool esvg_is_animate_base_internal(Edom_Tag *t);
void * esvg_animate_base_data_get(Edom_Tag *t);
Edom_Tag * esvg_animate_base_new(Esvg_Animate_Base_Descriptor *descriptor, Esvg_Type type, void *data);

Etch_Interpolator_Type esvg_animate_base_calc_mode_etch_to(Esvg_Calc_Mode c);
/* generated function */
void esvg_animate_base_init(void);
void esvg_animate_base_shutdown(void);

#endif

