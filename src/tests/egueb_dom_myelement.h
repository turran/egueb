#ifndef _EGUEB_DOM_MYELEMENT_H_
#define _EGUEB_DOM_MYELEMENT_H_

Egueb_Dom_Node * myelement_new(void);
void myelement_prop1_set(Egueb_Dom_Node *n, Egueb_Dom_String *s);
void myelement_dump(Egueb_Dom_Node *thiz, Eina_Bool deep);

#endif

