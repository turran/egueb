#ifndef _ESVG_PRIVATE_ANIMATE_BASE_H_
#define _ESVG_PRIVATE_ANIMATE_BASE_H_

#include "esvg_private_attribute_animation.h"

/* this is the callback the animation will call */
typedef void  (*Esvg_Animate_Base_Animation_Callback)(Edom_Tag *t,
		Ender_Element *e,
		Ender_Property *p,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *kdata,
		void *data);
typedef void (*Esvg_Animate_Base_Value_Etch_Data_To)(void *d,
		Etch_Data *data);

typedef struct _Esvg_Animate_Base_Context {
	Esvg_Attribute_Animation_Value value;
	Eina_Bool changed : 1;
} Esvg_Animate_Base_Context;

typedef Eina_Bool (*Esvg_Animate_Base_Value_Get)(const char *attr, void **value);
typedef void (*Esvg_Animate_Base_Value_Free)(void *value);
typedef Eina_Bool (*Esvg_Animate_Base_Animation_Generate)(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx);

typedef struct _Esvg_Animate_Base_Type_Descriptor
{
	/* to generate the values */
	Esvg_Animate_Base_Value_Get value_get;
	Esvg_Animate_Base_Value_Free value_free;
	/* to generate the animations */
	Esvg_Animate_Base_Animation_Generate animation_generate;
} Esvg_Animate_Base_Type_Descriptor;

typedef Eina_Bool (*Esvg_Animate_Base_Type_Descriptor_Get)(Edom_Tag *t,
		const char *name,
		Esvg_Animate_Base_Type_Descriptor **d);

typedef struct _Esvg_Animate_Base_Descriptor {
	/* the tag interface */
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	/* the animate_base interface */
	Esvg_Animate_Base_Type_Descriptor_Get type_descriptor_get;
} Esvg_Animate_Base_Descriptor;

Eina_Bool esvg_is_animate_base_internal(Edom_Tag *t);
void * esvg_animate_base_data_get(Edom_Tag *t);
Edom_Tag * esvg_animate_base_new(Esvg_Animate_Base_Descriptor *descriptor, Esvg_Type type, void *data);

Etch_Animation_Type esvg_animate_base_calc_mode_etch_to(Esvg_Calc_Mode c);

Etch_Animation * esvg_animate_base_animation_simple_add(Edom_Tag *t, Etch_Data_Type dt,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Animate_Base_Animation_Callback cb, void *data);
Etch_Animation * esvg_animate_base_animation_empty_add(Edom_Tag *t, Etch_Data_Type dt,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Esvg_Animate_Base_Animation_Callback cb, void *data);
void esvg_animate_base_animation_add_keyframe(Etch_Animation *a,
	Esvg_Animate_Base_Context *c,
	Etch_Data *etch_data,
	int64_t time, void *data);
void esvg_animate_base_animation_generate(Edom_Tag *t,
		Eina_List *values,
		Eina_List *times,
		Esvg_Animation_Context *actx,
		Esvg_Animate_Base_Context *abctx,
		Etch_Data_Type dt,
		Esvg_Animate_Base_Value_Etch_Data_To data_to,
		Esvg_Animate_Base_Animation_Callback cb,
		void *data);
/* generated function */
void esvg_animate_base_init(void);

#endif

