#ifndef _ESVG_PRIVATE_RENDERABLE_H_
#define _ESVG_PRIVATE_RENDERABLE_H_

/* referenceable */
typedef Enesim_Renderer * (*Esvg_Referenceable_Renderer_New)(Edom_Tag *t);
typedef Esvg_Element_Setup_Return (*Esvg_Referenceable_Setup)(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Renderer *current,
		Enesim_Error **error);

typedef Eina_Bool (*Esvg_Referenceable_Propagate)(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_ctx,
		const Esvg_Attribute_Presentation *attr,
		Enesim_Renderer *r,
		Enesim_Error **error);

typedef Eina_Bool (*Esvg_Referenceable_Reference_Add)(Edom_Tag *t, Ender_Element *e);
typedef Eina_Bool (*Esvg_Referenceable_Reference_Remove)(Edom_Tag *t, Ender_Element *e);

typedef struct _Esvg_Referenceable_Descriptor {
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
	/* the referenceable interface */
	Esvg_Referenceable_Setup setup;
	Esvg_Referenceable_Renderer_New renderer_new;
	Esvg_Referenceable_Propagate propagate;
	Esvg_Referenceable_Reference_Add reference_add;
	Esvg_Referenceable_Reference_Remove reference_remove;
} Esvg_Referenceable_Descriptor;

void * esvg_referenceable_data_get(Edom_Tag *t);
Edom_Tag * esvg_referenceable_new(Esvg_Referenceable_Descriptor *descriptor, Esvg_Type type, void *data);
Enesim_Renderer * esvg_referenceable_renderer_new(Edom_Tag *t);
void esvg_referenceable_renderer_set(Edom_Tag *t, Enesim_Renderer *r);
/* internal functions */
Eina_Bool esvg_is_referenceable_internal(Edom_Tag *t);

#endif
