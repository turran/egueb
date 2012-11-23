#ifndef _ESVG_PRIVATE_SVG_H_
#define _ESVG_PRIVATE_SVG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* FIXME for now */
#include "esvg_private_scriptor.h"

Etch * esvg_svg_etch_get(Ender_Element *e);
Ender_Element * esvg_svg_internal_element_find(Edom_Tag *t, const char *id);
void esvg_svg_element_get(Ender_Element *e, const char *uri, Ender_Element **el);
void esvg_svg_image_load(Ender_Element *e, const char *uri, Enesim_Surface **s, double width, double height);
Ender_Element * esvg_svg_svg_load(Ender_Element *e, const char *uri);
char * esvg_svg_uri_resolve(Ender_Element *e, const char *uri);
Enesim_Surface * esvg_svg_surface_new(Ender_Element *e, int w, int h);

void esvg_svg_internal_container_width_get(Edom_Tag *t, double *container_width);
void esvg_svg_internal_container_height_get(Edom_Tag *t, double *container_height);

/* application related functions */
void esvg_svg_go_to(Ender_Element *e, const char *uri);
void esvg_svg_script_alert(Ender_Element *e, const char *msg);


/* script related functions */
void esvg_svg_scriptor_register(Esvg_Scriptor_Descriptor *d, const char *type);
void esvg_svg_scriptor_unregister(Esvg_Scriptor_Descriptor *d, const char *type);
Esvg_Scriptor * esvg_svg_scriptor_get(Ender_Element *e, const char *type);

void esvg_svg_init(void);
void esvg_svg_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
