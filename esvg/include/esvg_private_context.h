#ifndef _ESVG_PRIVATE_CONTEXT_H_
#define _ESVG_PRIVATE_CONTEXT_H_

/* TODO on the context we should always pass the topmost svg
 * so we dont need to request it, maybe also the parent ...
 * should we destroy the queue thing?
 */

typedef struct _Esvg_Context Esvg_Context;

struct _Esvg_Context
{
	int run;
	Eina_Bool dequeuing;
	Eina_List *queue;
};

void esvg_context_init(Esvg_Context *thiz);
void esvg_context_shutdown(Esvg_Context *thiz);
void esvg_context_setup_enqueue(Esvg_Context *thiz, Edom_Tag *t);
void esvg_context_setup_dequeue(Esvg_Context *thiz);

#endif
