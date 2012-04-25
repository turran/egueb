#ifndef _ESVG_PRIVATE_GRADIENT_H_
#define _ESVG_PRIVATE_GRADIENT_H_

typedef struct _Esvg_Gradient_Context {
	Esvg_Gradient_Units units;
	Esvg_Spread_Method spread_method;
	Enesim_Matrix transform;
} Esvg_Gradient_Context;

typedef Eina_Bool (*Esvg_Gradient_Setup)(Edom_Tag *t,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Renderer *current,
		Esvg_Gradient_Context *gctx,
		Enesim_Error **error);

typedef struct _Esvg_Gradient_Descriptor {
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
	/* the renderable interface */
	Esvg_Gradient_Setup setup;
	Esvg_Referenceable_Renderer_New renderer_new;
} Esvg_Gradient_Descriptor;

void * esvg_gradient_data_get(Edom_Tag *t);
Edom_Tag * esvg_gradient_new(Esvg_Gradient_Descriptor *descriptor, Esvg_Type type, void *data);

#endif
