#ifndef _ESVG_PRIVATE_GRADIENT_H_
#define _ESVG_PRIVATE_GRADIENT_H_

typedef struct _Esvg_Element_Gradient_Context {
	Esvg_Element_Gradient_Units units;
	Esvg_Spread_Method spread_method;
	Enesim_Matrix transform;
} Esvg_Element_Gradient_Context;

typedef Eina_Bool (*Esvg_Element_Gradient_Propagate)(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Element_Gradient_Context *gctx,
		Enesim_Renderer *r,
		Enesim_Error **error);

typedef struct _Esvg_Element_Gradient_Descriptor {
	/* the tag interface */
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Cdata_Set cdata_set;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	Esvg_Element_Attribute_Animated_Fetch attribute_animated_fetch;
	/* the referenceable interface */
	Esvg_Referenceable_Setup setup;
	/* the paint server interface */
	Esvg_Paint_Server_Renderer_New renderer_new;
	/* our own interface */
	Esvg_Element_Gradient_Propagate propagate;
} Esvg_Element_Gradient_Descriptor;

Edom_Tag * esvg_element_gradient_new(Esvg_Element_Gradient_Descriptor *descriptor, Esvg_Type type, void *data);
void * esvg_element_gradient_data_get(Edom_Tag *t);
Edom_Tag * esvg_element_gradient_href_tag_get(Edom_Tag *t);
/* generated function */
void esvg_element_gradient_init(void);
void esvg_element_gradient_shutdown(void);

#endif
