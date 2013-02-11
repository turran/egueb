#ifndef _ESVG_PRIVATE_RENDERABLE_CONTAINER_H_
#define _ESVG_PRIVATE_RENDERABLE_CONTAINER_H_

typedef struct _Esvg_Renderable_Container Esvg_Renderable_Container;

Esvg_Renderable_Container * esvg_renderable_container_new(Ender_Element *e);
void esvg_renderable_container_free(Esvg_Renderable_Container *thiz);
void esvg_renderable_container_clear(Esvg_Renderable_Container *thiz);
void esvg_renderable_container_renderable_add(Esvg_Renderable_Container *thiz,
		Edom_Tag *t);
void esvg_renderable_container_renderable_remove(Esvg_Renderable_Container *thiz,
		Edom_Tag *t);

#endif
