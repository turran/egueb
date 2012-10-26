#ifndef _ESVG_ATTRIBUTE_PRESENTATION_H_
#define _ESVG_ATTRIBUTE_PRESENTATION_H_


#include "esvg_types.h"
#include "esvg_private_attribute.h"

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
	Esvg_Attribute_String clip_path;
	Esvg_Attribute_Color color;
	Esvg_Attribute_Number opacity;
	Esvg_Attribute_Paint fill;
	Esvg_Attribute_Paint stroke;
	Esvg_Attribute_Length stroke_width;
	Esvg_Attribute_Enum stroke_line_cap;
	Esvg_Attribute_Enum stroke_line_join;
	Esvg_Attribute_Number stroke_opacity;
	Esvg_Attribute_Number fill_opacity;
	Esvg_Attribute_Enum fill_rule;
	Esvg_Attribute_Enum visibility;
	Esvg_Attribute_Color stop_color;
	Esvg_Attribute_Number stop_opacity;
	Esvg_Attribute_Enum display;
	/* are they set? */
	/* how may are set ? */
	int sets;
	/* has something changed ? */
	Eina_Bool changed;
} Esvg_Attribute_Presentation;

void esvg_attribute_presentation_setup(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_cleanup(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_dump(Esvg_Attribute_Presentation *thiz);
void esvg_attribute_presentation_merge_rel(
		const Esvg_Attribute_Presentation *rel,
		const Esvg_Attribute_Presentation *s,
		Esvg_Attribute_Presentation *d);

#endif
