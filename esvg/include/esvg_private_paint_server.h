#ifndef _ESVG_PRIVATE_PAINT_SERVER_H_
#define _ESVG_PRIVATE_PAINT_SERVER_H_

typedef struct _Esvg_Paint_Server_Descriptor {
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
	Esvg_Referenceable_Setup setup;
	Esvg_Referenceable_Renderer_New renderer_new;
	Esvg_Referenceable_Propagate propagate;
	Esvg_Referenceable_Reference_Add reference_add;
	Esvg_Referenceable_Reference_Remove reference_remove;
} Esvg_Paint_Server_Descriptor;

void * esvg_paint_server_data_get(Edom_Tag *t);
Edom_Tag * esvg_paint_server_new(Esvg_Paint_Server_Descriptor *descriptor, Esvg_Type type, void *data);


#endif
