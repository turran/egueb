#ifndef _ESVG_PRIVATE_ANIMATION_H_
#define _ESVG_PRIVATE_ANIMATION_H_

#include "esvg_private_attribute_animation.h"

/* animation */
typedef struct _Esvg_Animation_Context {
	Esvg_Attribute_Animation_Target target;
	Esvg_Attribute_Animation_Timing timing;
	Eina_Bool changed : 1;
} Esvg_Animation_Context;

typedef Eina_Bool (*Esvg_Animation_Setup)(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Animation_Context *rctx,
		Enesim_Error **error);

typedef struct _Esvg_Animation_Descriptor {
	/* the tag interface */
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	/* the animation interface */
	Esvg_Animation_Setup setup;
} Esvg_Animation_Descriptor;

void * esvg_animation_data_get(Edom_Tag *t);
Edom_Tag * esvg_animation_new(Esvg_Animation_Descriptor *descriptor, Esvg_Type type, void *data);
/* generated function */
void esvg_animation_init(void);

#endif
