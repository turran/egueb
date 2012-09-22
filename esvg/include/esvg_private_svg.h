#ifndef _ESVG_PRIVATE_SVG_H_
#define _ESVG_PRIVATE_SVG_H_

Etch * esvg_svg_etch_get(Ender_Element *e);
Ender_Element * esvg_svg_internal_element_find(Edom_Tag *t, const char *id);
void esvg_svg_element_get(Ender_Element *e, const char *uri, Ender_Element **el);
void esvg_svg_image_load(Ender_Element *e, const char *uri, Enesim_Surface **s, const char *options);
char * esvg_svg_uri_resolve(Ender_Element *e, const char *uri);

void esvg_svg_internal_container_width_get(Edom_Tag *t, double *container_width);
void esvg_svg_internal_container_height_get(Edom_Tag *t, double *container_height);


#endif
