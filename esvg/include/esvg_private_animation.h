#ifndef _ESVG_PRIVATE_ANIMATION_H_
#define _ESVG_PRIVATE_ANIMATION_H_

/* animation */
typedef struct _Esvg_Animation_Context {
} Esvg_Animation_Context;

typedef Eina_Bool (*Esvg_Animation_Setup)(Edom_Tag *t,
		Esvg_Element_Context *ctx,
		Esvg_Animation_Context *rctx,
		Enesim_Error **error);

typedef struct _Esvg_Animation_Descriptor {
	/* the tag interface */
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Cdata_Set cdata_set;
	Edom_Tag_Text_Set text_set;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	Esvg_Element_Clone clone;
	/* the animation interface */
	Esvg_Animation_Setup setup;
} Esvg_Animation_Descriptor;

void * esvg_animation_data_get(Edom_Tag *t);
Edom_Tag * esvg_animation_new(Esvg_Animation_Descriptor *descriptor, Esvg_Type type, void *data);

#endif
