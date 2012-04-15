#ifndef _ESVG_PRIVATE_RENDERABLE_H_
#define _ESVG_PRIVATE_RENDERABLE_H_

/* renderable */
typedef Enesim_Renderer * (*Esvg_Renderable_Renderer_Get)(Edom_Tag *t,
		const Esvg_Element_State *state,
		const Esvg_Attribute_Presentation *attr);

typedef struct _Esvg_Renderable_Descriptor {
	/* the tag interface */
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	Edom_Tag_Attribute_Set attribute_set;
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Cdata_Set cdata_set;
	Edom_Tag_Text_Set text_set;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Clone clone;
	Esvg_Element_Setup setup;
	/* the renderable interface */
	Esvg_Renderable_Renderer_Get renderer_get;
} Esvg_Renderable_Descriptor;

void * esvg_renderable_data_get(Edom_Tag *t);
Edom_Tag * esvg_renderable_new(Esvg_Renderable_Descriptor *descriptor, Esvg_Type type, void *data);

#endif
