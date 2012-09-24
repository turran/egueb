#ifndef _ESVG_PRIVATE_RENDERABLE_CONTAINER_H_
#define _ESVG_PRIVATE_RENDERABLE_CONTAINER_H_

typedef void (*Esvg_Renderable_Container_Renderable_Add)(Edom_Tag *t,
		Edom_Tag *renderable);
typedef void (*Esvg_Renderable_Container_Renderable_Remove)(Edom_Tag *t,
		Edom_Tag *renderable);

typedef struct _Esvg_Renderable_Container_Descriptor {
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
	Esvg_Element_Attribute_Animated_Fetch attribute_animated_fetch;
	/* the renderable interface */
	Esvg_Renderable_Setup setup;
	Esvg_Renderable_Renderer_Get renderer_get;
	Esvg_Renderable_Renderer_Propagate renderer_propagate;
} Esvg_Renderable_Descriptor;

void * esvg_renderable_container_data_get(Edom_Tag *t);
Edom_Tag * esvg_renderable_container_new(Esvg_Renderable_Container_Descriptor *descriptor, Esvg_Type type, void *data);

#endif
