#ifndef _ESVG_ATTRIBUTE_PRESENTATION_H_
#define _ESVG_ATTRIBUTE_PRESENTATION_H_

#include <Eina.h>
#include <Edom.h>
#include "esvg_types.h"

/* ‘alignment-baseline’,
 * ‘baseline-shift’,
 * ‘clip-rule’,
 * ‘clip’,
 * ‘color-interpolation-filters’,
 * ‘color-interpolation’,
 * ‘color-profile’,
 * ‘color-rendering’,
 * ‘cursor’,
 * ‘direction’,
 * ‘display’,
 * ‘dominant-baseline’,
 * ‘enable-background’,
 * ‘filter’,
 * ‘flood-color’,
 * ‘flood-opacity’,
 * ‘font-family’,
 * ‘font-size-adjust’,
 * ‘font-size’,
 * ‘font-stretch’,
 * ‘font-style’,
 * ‘font-variant’,
 * ‘font-weight’,
 * ‘glyph-orientation-horizontal’,
 * ‘glyph-orientation-vertical’,
 * ‘image-rendering’,
 * ‘kerning’,
 * ‘letter-spacing’,
 * ‘lighting-color’,
 * ‘marker-end’,
 * ‘marker-mid’,
 * ‘marker-start’,
 * ‘mask’,
 * ‘overflow’,
 * ‘pointer-events’,
 * ‘shape-rendering’,
 * ‘stroke-dasharray’,
 * ‘stroke-dashoffset’,
 * ‘stroke-miterlimit’,
 * ‘text-anchor’,
 * ‘text-decoration’,
 * ‘text-rendering’,
 * ‘unicode-bidi’,
 * ‘word-spacing’
 * ‘writing-mode’
 */

typedef struct _Esvg_Attribute_Presentation {
	char *clip_path;
	Esvg_Color color;
	double opacity;
	Esvg_Paint fill;
	Esvg_Paint stroke;
	Esvg_Length stroke_width;
	Esvg_Stroke_Line_Cap stroke_line_cap;
	Esvg_Stroke_Line_Join stroke_line_join;
	double stroke_opacity;
	double fill_opacity;
	Esvg_Fill_Rule fill_rule;
	Eina_Bool visibility;
	Esvg_Color stop_color;
	double stop_opacity;
	/* are they set? */
	Eina_Bool clip_path_set;
	Eina_Bool color_set;
	Eina_Bool opacity_set;
	Eina_Bool fill_set;
	Eina_Bool stroke_set;
	Eina_Bool stroke_width_set;
	Eina_Bool stroke_line_cap_set;
	Eina_Bool stroke_line_join_set;
	Eina_Bool stroke_opacity_set;
	Eina_Bool fill_opacity_set;
	Eina_Bool fill_rule_set;
	Eina_Bool visibility_set;
	Eina_Bool stop_color_set;
	Eina_Bool stop_opacity_set;
	/* how may are set ? */
	int sets;
	/* has something changed ? */
	Eina_Bool changed;
} Esvg_Attribute_Presentation;

void esvg_attribute_presentation_setup(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_copy(const Esvg_Attribute_Presentation *dst,
		const Esvg_Attribute_Presentation *src);
void esvg_attribute_presentation_merge(const Esvg_Attribute_Presentation *state,
		const Esvg_Attribute_Presentation *parent,
		Esvg_Attribute_Presentation *d);

void esvg_attribute_presentation_clip_path_set(Esvg_Attribute_Presentation *thiz, const char *clip_path);
void esvg_attribute_presentation_clip_path_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_color_set(Esvg_Attribute_Presentation *thiz, const Esvg_Color *color);
void esvg_attribute_presentation_color_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_opacity_set(Esvg_Attribute_Presentation *thiz, double opacity);
void esvg_attribute_presentation_fill_set(Esvg_Attribute_Presentation *thiz, const Esvg_Paint *fill);
void esvg_attribute_presentation_fill_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_fill_opacity_set(Esvg_Attribute_Presentation *thiz, double fill_opacity);
void esvg_attribute_presentation_fill_opacity_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_fill_rule_set(Esvg_Attribute_Presentation *thiz, Esvg_Fill_Rule rule);
void esvg_attribute_presentation_fill_rule_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_stroke_set(Esvg_Attribute_Presentation *thiz, const Esvg_Paint *stroke);
void esvg_attribute_presentation_stroke_opacity_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_stroke_width_set(Esvg_Attribute_Presentation *thiz, const Esvg_Length *stroke_width);
void esvg_attribute_presentation_stroke_opacity_set(Esvg_Attribute_Presentation *thiz, double stroke_opacity);
void esvg_attribute_presentation_stroke_line_cap_set(Esvg_Attribute_Presentation *thiz, Esvg_Stroke_Line_Cap cap);
void esvg_attribute_presentation_stroke_line_join_set(Esvg_Attribute_Presentation *thiz, Esvg_Stroke_Line_Join join);
void esvg_attribute_presentation_stroke_line_join_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_stroke_line_cap_unset(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_visibility_set(Esvg_Attribute_Presentation *thiz, Eina_Bool visibility);
void esvg_attribute_presentation_visibility_unset(Esvg_Attribute_Presentation *thiz);

void esvg_attribute_presentation_stop_color_set(Esvg_Attribute_Presentation *thiz, const Esvg_Color *stop_color);
void esvg_attribute_presentation_stop_color_unset(Esvg_Attribute_Presentation *thiz);

void esvg_attribute_presentation_stop_opacity_set(Esvg_Attribute_Presentation *thiz, double stop_opacity);
void esvg_attribute_presentation_stop_opacity_unset(Esvg_Attribute_Presentation *thiz, double stop_opacity);

void esvg_attribute_presentation_dump(Esvg_Attribute_Presentation *thiz);

#endif
