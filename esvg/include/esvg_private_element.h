#ifndef _ESVG_PRIVATE_ELEMENT_H_
#define _ESVG_PRIVATE_ELEMENT_H_

/* element */
typedef struct _Esvg_Element_State {
	double dpi_x;
	double dpi_y;
	double viewbox_w; /* FIXME remove this */
	double viewbox_h; /* FIXME remove this */
	Enesim_Rectangle viewbox;
	Enesim_Matrix transform;
	char *style;
	char *id;
	char *class;
} Esvg_Element_State;

/* FIXME change this to be the esvg element? */
typedef void (*Esvg_Element_Initialize)(Ender_Element *e);

typedef Eina_Bool (*Esvg_Element_Setup)(Edom_Tag *t,
		Esvg_Element_State *state,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error);
typedef void (*Esvg_Element_Clone)(Edom_Tag *r, Edom_Tag *dst);

typedef struct _Esvg_Element_Descriptor {
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
} Esvg_Element_Descriptor;

void * esvg_element_data_get(Edom_Tag *t);
Edom_Tag * esvg_element_new(Esvg_Element_Descriptor *descriptor, Esvg_Type type, void *data);
Eina_Bool esvg_element_setup(Edom_Tag *t, const Esvg_Element_State *state, const Esvg_Attribute_Presentation *attr, Enesim_Surface *s, Enesim_Error **error);
void esvg_element_state_compose(Edom_Tag *t, const Esvg_Element_State *s, Esvg_Element_State *d);

#endif
