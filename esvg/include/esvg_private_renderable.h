#ifndef _ESVG_PRIVATE_RENDERABLE_H_
#define _ESVG_PRIVATE_RENDERABLE_H_

/* renderable */
typedef Enesim_Renderer * (*Esvg_Renderable_Renderer_Get)(Edom_Tag *t);

typedef struct _Esvg_Renderable_Descriptor {
	/* the tag interface */
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	Edom_Tag_Attribute_Get attribute_get;
	Edom_Tag_Cdata_Set cdata_set;
	Edom_Tag_Text_Set text_set;
	Edom_Tag_Free free;
	/* the element interface */
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	Esvg_Element_Clone clone;
	Esvg_Element_Setup setup;
	/* the renderable interface */
	Esvg_Renderable_Renderer_Get renderer_get;
} Esvg_Renderable_Descriptor;

void * esvg_renderable_data_get(Edom_Tag *t);
Edom_Tag * esvg_renderable_new(Esvg_Renderable_Descriptor *descriptor, Esvg_Type type, void *data);

/* FIXME old shape structs, fix this */
typedef struct _Esvg_Shape_Enesim_State {
	/* common */
	Enesim_Color color;
	/* stroke */
	Enesim_Shape_Draw_Mode draw_mode;
	Enesim_Shape_Stroke_Cap stroke_cap;
	Enesim_Shape_Stroke_Join stroke_join;
	Enesim_Color stroke_color;
	Enesim_Renderer *stroke_renderer;
	double stroke_weight;
	/* fill */
	Enesim_Renderer *fill_renderer;
	Enesim_Color fill_color;
	Enesim_Shape_Fill_Rule fill_rule;
} Esvg_Shape_Enesim_State;

typedef Eina_Bool (*Esvg_Shape_Enesim_State_Calculate)(Enesim_Renderer *r,
		Esvg_Element_State *estate,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Shape_Enesim_State *dstate,
		void *data);

typedef Enesim_Renderer * (*Esvg_Shape_Renderer_Get)(Enesim_Renderer *);
typedef Eina_Bool (*Esvg_Shape_Setup)(Enesim_Renderer *r, const Esvg_Element_State *estate,
		const Esvg_Shape_Enesim_State *dstate);

#endif
