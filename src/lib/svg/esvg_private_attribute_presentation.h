#ifndef _ESVG_ATTRIBUTE_PRESENTATION_H_
#define _ESVG_ATTRIBUTE_PRESENTATION_H_


#include "egueb_svg_types.h"
#include "egueb_svg_attribute_private.h"

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

typedef struct _Egueb_Svg_Attribute_Presentation {
	Egueb_Svg_Attribute_Clip_Path clip_path;
	Egueb_Svg_Attribute_Color color;
	Egueb_Svg_Attribute_Number opacity;
	Egueb_Svg_Attribute_Paint fill;
	Egueb_Svg_Attribute_Paint stroke;
	Egueb_Svg_Attribute_Length stroke_width;
	Egueb_Svg_Attribute_Enum stroke_line_cap;
	Egueb_Svg_Attribute_Enum stroke_line_join;
	Egueb_Svg_Attribute_Number stroke_opacity;
	Egueb_Svg_Attribute_Number fill_opacity;
	Egueb_Svg_Attribute_Enum fill_rule;
	Egueb_Svg_Attribute_Enum visibility;
	Egueb_Svg_Attribute_Color stop_color;
	Egueb_Svg_Attribute_Number stop_opacity;
	Egueb_Svg_Attribute_Enum display;
	/* are they set? */
	/* how may are set ? */
	int sets;
	/* has something changed ? */
	Eina_Bool changed;
} Egueb_Svg_Attribute_Presentation;

void egueb_svg_attribute_presentation_setup(Egueb_Svg_Attribute_Presentation *thiz);
void egueb_svg_attribute_presentation_dump(Egueb_Svg_Attribute_Presentation *thiz);
void egueb_svg_attribute_presentation_merge_rel(
		const Egueb_Svg_Attribute_Presentation *rel,
		const Egueb_Svg_Attribute_Presentation *s,
		Egueb_Svg_Attribute_Presentation *d);

#endif
