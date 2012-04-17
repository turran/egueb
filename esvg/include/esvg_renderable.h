#ifndef _ESVG_RENDERABLE_H_
#define _ESVG_RENDERABLE_H_

EAPI Eina_Bool esvg_is_renderable(Ender_Element *e);
EAPI Enesim_Renderer * esvg_renderable_renderer_get(Ender_Element *e);
EAPI void esvg_renderable_x_dpi_set(Ender_Element *e, double x_dpi);
EAPI void esvg_renderable_x_dpi_get(Ender_Element *e, double *x_dpi);
EAPI void esvg_renderable_y_dpi_set(Ender_Element *e, double y_dpi);
EAPI void esvg_renderable_y_dpi_get(Ender_Element *e, double *y_dpi);
EAPI void esvg_renderable_container_width_set(Ender_Element *e, double container_width);
EAPI void esvg_renderable_container_width_get(Ender_Element *e, double *container_width);
EAPI void esvg_renderable_container_height_set(Ender_Element *e, double container_height);
EAPI void esvg_renderable_container_height_get(Ender_Element *e, double *container_height);

#endif
