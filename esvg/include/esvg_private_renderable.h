#ifndef _ESVG_PRIVATE_RENDERABLE_H_
#define _ESVG_PRIVATE_RENDERABLE_H_

/* renderable */
typedef Enesim_Renderer * (*Esvg_Renderable_Renderer_Get)(Edom_Tag *t);
typedef Esvg_Element_Setup_Return (*Esvg_Renderable_Setup)(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error);

typedef Eina_Bool (*Esvg_Renderable_Renderer_Propagate)(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error);

typedef struct _Esvg_Renderable_Descriptor {
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
	Esvg_Element_Attribute_Animation_Add animation_add;
	Esvg_Element_Attribute_Animation_Remove animation_remove;
	/* the renderable interface */
	Esvg_Renderable_Setup setup;
	Esvg_Renderable_Renderer_Get renderer_get;
	Esvg_Renderable_Renderer_Propagate renderer_propagate;
} Esvg_Renderable_Descriptor;

void * esvg_renderable_data_get(Edom_Tag *t);
Edom_Tag * esvg_renderable_new(Esvg_Renderable_Descriptor *descriptor, Esvg_Type type, void *data);
void esvg_renderable_implementation_renderer_get(Edom_Tag *t, Enesim_Renderer **r);
/* internal functions */
/* TODO this property should not be exposed */
void esvg_renderable_internal_renderer_get(Edom_Tag *t, Enesim_Renderer **r);
Eina_Bool esvg_is_renderable_internal(Edom_Tag *t);

void esvg_renderable_internal_container_width_get(Edom_Tag *t, double *container_width);
void esvg_renderable_internal_container_height_get(Edom_Tag *t, double *container_height);

/* old ones */
typedef Eina_Bool (*Esvg_Renderable_Context_Calculate)(Enesim_Renderer *r,
		Esvg_Element_Context *estate,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *dstate,
		void *data);

typedef Enesim_Renderer * (*Esvg_Shape_Renderer_Get)(Enesim_Renderer *);
typedef Eina_Bool (*Esvg_Shape_Setup)(Enesim_Renderer *r, const Esvg_Element_Context *estate,
		const Esvg_Renderable_Context *dstate);

#endif
