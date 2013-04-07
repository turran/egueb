#ifndef _EGUEB_DOM_DESCRIPTOR_H_
#define _EGUEB_DOM_DESCRIPTOR_H_

typedef struct _Egueb_Dom_Descriptor Egueb_Dom_Descriptor;

EAPI Egueb_Dom_Descriptor * egueb_dom_descriptor_new(void);
EAPI void egueb_dom_descriptor_ender_descriptor_set(Egueb_Dom_Descriptor *thiz,
		Ender_Descriptor *ed);
EAPI Ender_Descriptor * egueb_dom_descriptor_ender_descriptor_get(
		Egueb_Dom_Descriptor *thiz);

EAPI void egueb_dom_descriptor_object_descriptor_set(Egueb_Dom_Descriptor *thiz,
		Enesim_Object_Descriptor *eod);
EAPI Enesim_Object_Descriptor * egueb_dom_descriptor_object_descriptor_get(
		Egueb_Dom_Descriptor *thiz);

#endif
