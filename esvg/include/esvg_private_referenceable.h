#ifndef _ESVG_PRIVATE_REFERENCEABLE_H_
#define _ESVG_PRIVATE_REFERENCEABLE_H_

typedef struct _Esvg_Referenceable_Reference
{
	Edom_Tag *t;
	Edom_Tag *referencer;
	/* instead of the renderer this could be just a void *
	 * where the child classes should fill this with its
	 * own data
	 */
	void *data;
} Esvg_Referenceable_Reference;

typedef Esvg_Element_Setup_Return (*Esvg_Referenceable_Setup)(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error);

typedef Eina_Bool (*Esvg_Referenceable_Propagate)(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_ctx,
		const Esvg_Attribute_Presentation *attr,
		void *data,
		Enesim_Error **error);

typedef void (*Esvg_Referenceable_Cleanup)(Edom_Tag *t);

typedef Eina_Bool (*Esvg_Referenceable_Reference_Add)(Edom_Tag *t, Esvg_Referenceable_Reference *rr);
typedef void (*Esvg_Referenceable_Reference_Remove)(Edom_Tag *t, Esvg_Referenceable_Reference *rr);

typedef struct _Esvg_Referenceable_Descriptor {
	/* the tag interface */
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Cdata_Set cdata_set;
	Edom_Tag_Text_Set text_set;
	Edom_Tag_Text_Get text_get;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	Esvg_Element_Attribute_Animated_Fetch attribute_animated_fetch;
	/* the referenceable interface */
	Esvg_Referenceable_Setup setup;
	Esvg_Referenceable_Cleanup cleanup;
	Esvg_Referenceable_Propagate propagate;
	Esvg_Referenceable_Reference_Add reference_add;
	Esvg_Referenceable_Reference_Remove reference_remove;
} Esvg_Referenceable_Descriptor;

typedef Eina_Bool (*Esvg_Referenceable_Cb)(Edom_Tag *t, Esvg_Referenceable_Reference *rr, void *data);

void * esvg_referenceable_data_get(Edom_Tag *t);
Edom_Tag * esvg_referenceable_new(Esvg_Referenceable_Descriptor *descriptor, Esvg_Type type, void *data);
Esvg_Referenceable_Reference * esvg_referenceable_reference_add(Edom_Tag *t,
		Edom_Tag *referencer);
void esvg_referenceable_reference_remove(Edom_Tag *t, Esvg_Referenceable_Reference *rr);
void esvg_referenceable_reference_foreach(Edom_Tag *t, Esvg_Referenceable_Cb cb, void *data);
void esvg_referenceable_reference_propagate(Esvg_Referenceable_Reference *rr,
		Esvg_Context *c,
		Enesim_Error **error);
/* internal functions */
Eina_Bool esvg_is_referenceable_internal(Edom_Tag *t);
/* generated function */
void esvg_referenceable_init(void);

#endif
