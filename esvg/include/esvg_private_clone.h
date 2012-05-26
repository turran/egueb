#ifndef _ESVG_PRIVATE_CLONE_H_
#define _ESVG_PRIVATE_CLONE_H_

typedef struct _Esvg_Clone
{
	Ender_Element *ref;
	Ender_Element *our;
} Esvg_Clone;

Esvg_Clone * esvg_clone_new(Ender_Element *e);

#endif
