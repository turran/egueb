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


typedef Eina_Bool (*Esvg_Renderable_Damage_Cb)(Ender_Element *e, Eina_Rectangle *damage,
		void *data);
EAPI void esvg_renderable_damages_get(Ender_Element *e, Esvg_Renderable_Damage_Cb cb, void *data);
EAPI Eina_Bool esvg_renderable_draw(Ender_Element *e, Enesim_Surface *s,
		Eina_Rectangle *clip, int x, int y, Enesim_Error **error);
EAPI Eina_Bool esvg_renderable_draw_list(Ender_Element *e, Enesim_Surface *s,
		Eina_List *clips, int x, int y, Enesim_Error **error);

#endif
