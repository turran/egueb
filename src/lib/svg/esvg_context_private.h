#ifndef _ESVG_PRIVATE_CONTEXT_H_
#define _ESVG_PRIVATE_CONTEXT_H_

/* TODO on the context we should always pass the topmost svg
 * so we dont need to request it, maybe also the parent ...
 * should we destroy the queue thing?
 */

typedef struct _Egueb_Svg_Context Egueb_Svg_Context;

struct _Egueb_Svg_Context
{
	int run;
	Eina_Bool dequeuing;
	Eina_List *queue;
};

void egueb_svg_context_init(Egueb_Svg_Context *thiz);
void egueb_svg_context_shutdown(Egueb_Svg_Context *thiz);
void egueb_svg_context_setup_enqueue(Egueb_Svg_Context *thiz, Egueb_Dom_Tag *t);
void egueb_svg_context_setup_dequeue(Egueb_Svg_Context *thiz);

#endif
