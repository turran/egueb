#ifndef _ESVG_PRIVATE_SVG_H_
#define _ESVG_PRIVATE_SVG_H_

#if 0
/* This svg external concept is useful for the case where the topmost svg needs
 * to process external svgs, like an <image> with a href:xlink set to a svg file
 */
typedef struct _Esvg_Svg_External_Descriptor Esvg_Svg_External_Descriptor;
typedef struct _Esvg_Svg_External Esvg_Svg_External;

/* we need a way to define a pull-surface, that is, when requesting an area of a surface to draw
 * it should pick a renderer and draw there only if it needs to, something like a 'cache renderer'
 * the contents are cached in a surface and if the inner renderer needs to redraw there then it
 * draw there, if not, it just picks up the cached area
 */
typedef void (*Esvg_Svg_External_Damage)(Esvg_Svg_External *thiz, Esvg_Svg_Damage_Cb cb, void *data);
typedef void (*Esvg_Svg_External_Draw)(Esvg_Svg_External *thiz, Eina_List *damages);
struct _Esvg_Svg_External_Descriptor
{
	Esvg_Svg_External_Damage damage;
	Esvg_Svg_External_Draw draw;
};

struct _Esvg_Svg_External
{
	Ender_Element *svg;
	Edom_Tag *owner;
	Enesim_Surface *s;
};

Esvg_Svg_External * esvg_svg_external_new(Ender_Element *svg, Edom_Tag *t, Enesim_Surface *s);
void esvg_svg_external_add(Ender_Element *e, Esvg_Svg_External *external);
void esvg_svg_external_remove(Ender_Element *e, Esvg_Svg_External *external);
void esvg_svg_external_free(Esvg_Svg_External *external);
#endif

Etch * esvg_svg_etch_get(Ender_Element *e);
Ender_Element * esvg_svg_internal_element_find(Edom_Tag *t, const char *id);
void esvg_svg_element_get(Ender_Element *e, const char *uri, Ender_Element **el);
void esvg_svg_image_load(Ender_Element *e, const char *uri, Enesim_Surface **s, double width, double height);
char * esvg_svg_uri_resolve(Ender_Element *e, const char *uri);
Enesim_Surface * esvg_svg_surface_new(Ender_Element *e, int w, int h);

void esvg_svg_internal_container_width_get(Edom_Tag *t, double *container_width);
void esvg_svg_internal_container_height_get(Edom_Tag *t, double *container_height);


#endif
