#ifndef _ESVG_PRIVATE_REFERENCE_H_
#define _ESVG_PRIVATE_REFERENCE_H_

typedef struct _Esvg_Reference
{
	Ender_Element *ref;
	Ender_Element *our;
} Esvg_Reference;

Esvg_Reference * esvg_reference_new(Ender_Element *e);

#endif
