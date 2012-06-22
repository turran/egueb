#ifndef _ESVG_PRIVATE_ANIMATE_BASE_H_
#define _ESVG_PRIVATE_ANIMATE_BASE_H_

#include "esvg_private_attribute_animation.h"

typedef void * (*Esvg_Animate_Base_Value_Get)(const char *attr);
typedef void (*Esvg_Animate_Base_Value_Free)(void *value);

typedef struct _Esvg_Animate_Base_Context {
	Esvg_Attribute_Animation_Value value;
	Eina_Bool changed : 1;
} Esvg_Animate_Base_Context;

typedef Eina_Bool (*Esvg_Animate_Base_Setup)(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Enesim_Error **error);

typedef struct _Esvg_Animate_Base_Descriptor {
	/* the tag interface */
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	/* the animate_base interface */
	Esvg_Animate_Base_Setup setup;
} Esvg_Animate_Base_Descriptor;

void * esvg_animate_base_data_get(Edom_Tag *t);
Edom_Tag * esvg_animate_base_new(Esvg_Animate_Base_Descriptor *descriptor, Esvg_Type type, void *data);

Etch_Animation_Type esvg_animate_base_calc_mode_etch_to(Esvg_Calc_Mode c);
Eina_Bool esvg_animate_base_times_generate(Esvg_Animation_Context *ac,
		Esvg_Animate_Base_Context *c,
		Eina_List *values,
		Eina_List **times);
void esvg_animate_base_times_free(Eina_List *times);
Eina_Bool esvg_animate_base_values_generate(Esvg_Animate_Base_Context *c,
		Esvg_Animate_Base_Value_Get get_cb,
		Eina_List **values,
		Eina_Bool *has_from);
void esvg_animate_base_values_free(Eina_List *values, Esvg_Animate_Base_Value_Free free_cb);

/* generated function */
void esvg_animate_base_init(void);

#endif

