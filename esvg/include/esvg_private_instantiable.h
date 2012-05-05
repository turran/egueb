#ifndef _ESVG_PRIVATE_INSTANTIABLE_H_
#define _ESVG_PRIVATE_INSTANTIABLE_H_

typedef struct _Esvg_Instantiable_Descriptor {
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
	Esvg_Renderable_Setup setup;
	Esvg_Renderable_Renderer_Get renderer_get;
	Esvg_Renderable_Renderer_Propagate renderer_propagate;
} Esvg_Instantiable_Descriptor;

void * esvg_instantiable_data_get(Edom_Tag *t);
Eina_Bool esvg_is_instantiable_internal(Edom_Tag *t);
Edom_Tag * esvg_instantiable_new(Esvg_Instantiable_Descriptor *descriptor, Esvg_Type type, void *data);

#endif
