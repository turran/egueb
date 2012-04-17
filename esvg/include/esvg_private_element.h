#ifndef _ESVG_PRIVATE_ELEMENT_H_
#define _ESVG_PRIVATE_ELEMENT_H_

/* element */
typedef struct _Esvg_Element_Context {
	double dpi_x;
	double dpi_y;
	Esvg_View_Box viewbox;
	Enesim_Matrix transform;
} Esvg_Element_Context;

typedef void (*Esvg_Element_Initialize)(Ender_Element *e);
typedef Eina_Bool (*Esvg_Element_Attribute_Set)(Ender_Element *e, const char *key, const char *value);

typedef Eina_Bool (*Esvg_Element_Setup)(Edom_Tag *t,
		const Esvg_Element_Context *parent_ctx,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error);
typedef void (*Esvg_Element_Clone)(Edom_Tag *r, Edom_Tag *dst);

typedef struct _Esvg_Element_Descriptor {
	/* the tag interface */
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Cdata_Set cdata_set;
	Edom_Tag_Text_Set text_set;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Attribute_Set attribute_set;
	Esvg_Element_Initialize initialize;
	Esvg_Element_Clone clone;
	Esvg_Element_Setup setup;
} Esvg_Element_Descriptor;

void * esvg_element_data_get(Edom_Tag *t);
Edom_Tag * esvg_element_new(Esvg_Element_Descriptor *descriptor, Esvg_Type type, void *data);

void esvg_element_initialize(Ender_Element *e);
Esvg_Type esvg_element_type_get_internal(Edom_Tag *t);
Eina_Bool esvg_is_element_internal(Edom_Tag *t);

void esvg_element_state_compose(Edom_Tag *t, const Esvg_Element_Context *s, Esvg_Element_Context *d);

#endif
