#ifndef _EGUEB_SMIL_PRIVATE_ANIMATE_BASE_H_
#define _EGUEB_SMIL_PRIVATE_ANIMATE_BASE_H_

#include "egueb_smil_private_attribute_animation.h"

typedef struct _Egueb_Smil_Animate_Base_Context {
	Egueb_Smil_Attribute_Animation_Value value;
	Eina_Bool changed : 1;
} Egueb_Smil_Animate_Base_Context;

typedef Eina_Bool (*Egueb_Smil_Animate_Base_Attribute_Descriptor_Get)(Egueb_Dom_Tag *t,
		const char *name,
		Egueb_Smil_Attribute_Animated_Descriptor **d);

typedef struct _Egueb_Smil_Animate_Base_Descriptor {
	/* the tag interface */
	Egueb_Dom_Tag_Attribute_Get attribute_get;
	Egueb_Dom_Tag_Free free;
	/* the element interface */
	Egueb_Smil_Element_Initialize initialize;
	Egueb_Smil_Element_Attribute_Set attribute_set;
	/* the animate_base interface */
	Egueb_Smil_Animate_Base_Attribute_Descriptor_Get type_descriptor_get;
} Egueb_Smil_Animate_Base_Descriptor;

Eina_Bool egueb_smil_is_animate_base_internal(Egueb_Dom_Tag *t);
void * egueb_smil_animate_base_data_get(Egueb_Dom_Tag *t);
Egueb_Dom_Tag * egueb_smil_animate_base_new(Egueb_Smil_Animate_Base_Descriptor *descriptor, Egueb_Smil_Type type, void *data);

Etch_Interpolator_Type egueb_smil_animate_base_calc_mode_etch_to(Egueb_Smil_Calc_Mode c);
/* generated function */
void egueb_smil_animate_base_init(void);
void egueb_smil_animate_base_shutdown(void);

#endif

