#ifndef _EGUEB_DOM_DESCRIPTOR_H_
#define _EGUEB_DOM_DESCRIPTOR_H_

typedef Eina_Error (*Egueb_Dom_Descriptor_Accessor)(Egueb_Dom_Node *n, void *value);
typedef Eina_Error (*Egueb_Dom_Descriptor_Unsetter)(Egueb_Dom_Node *n);

typedef struct _Egueb_Dom_Element_Descriptor Egueb_Dom_Element_Descriptor;

typedef struct _Egueb_Dom_Descriptor_Attribute Egueb_Dom_Descriptor_Attribute;

EAPI Egueb_Dom_Descriptor * egueb_dom_element_descriptor_new(
		Egueb_Dom_Descriptor *parent,
		const char *name,
		Egueb_Dom_Element_Klass_Init init, void *klass);
EAPI const Egueb_Dom_Descriptor_Attribute * egueb_dom_descriptor_attribute_add(
		Egueb_Dom_Descriptor *thiz, const char *name,
		Egueb_Dom_Descriptor_Accessor set,
		Egueb_Dom_Descriptor_Accessor get,
		Egueb_Dom_Descriptor_Unsetter unset);

#endif
