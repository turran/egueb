#ifndef _ESVG_PRIVATE_RENDERABLE_BEHAVIOUR_H_
#define _ESVG_PRIVATE_RENDERABLE_BEHAVIOUR_H_

/* FIXME rename this to match the namespace, i.e Esvg_Renderable_Behaviour_Context */
typedef struct _Esvg_Renderable_Context {
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
} Esvg_Renderable_Context;

/* given that the renderables can be childs of several type of parents
 * it is desired to define the behaviour of such renderables, for example
 * shapes being child of a clippath must not process the fill/stroke
 * attributes. A shape being child of a def should not create a renderer
 * or process anything, etc
 */
/* FIXME we dont add the Element Context, some issues with the
 * recursive includes */
typedef void (*Esvg_Renderable_Behaviour_Context_Set)(Edom_Tag *t,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		void *data);

typedef struct _Esvg_Renderable_Behaviour
{
	Esvg_Renderable_Behaviour_Context_Set context_set;
	void *data;
} Esvg_Renderable_Behaviour;

#endif
