/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "egueb_svg_main_private.h"
#include "egueb_svg_main.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_text_content.h"
#include "egueb_svg_attr_matrix.h"
#include "egueb_svg_text_content_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_ELEMENT_DESCRIPTOR,
		Egueb_Svg_Text_Content, Egueb_Svg_Text_Content_Class,
		egueb_svg_text_content);

static void _egueb_svg_text_content_class_init(void *k)
{
}

static void _egueb_svg_text_content_class_deinit(void *k)
{
}

static void _egueb_svg_text_content_instance_init(void *o)
{
	Egueb_Svg_Text_Content *thiz;

	thiz = EGUEB_SVG_TEXT_CONTENT(o);
}

static void _egueb_svg_text_content_instance_deinit(void *o)
{
	Egueb_Svg_Text_Content *thiz;

	thiz = EGUEB_SVG_TEXT_CONTENT(o);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
#if 0
// lengthAdjust Types
  const unsigned short LENGTHADJUST_UNKNOWN = 0;
  const unsigned short LENGTHADJUST_SPACING = 1;
  const unsigned short LENGTHADJUST_SPACINGANDGLYPHS = 2;

  readonly attribute SVGAnimatedLength textLength;
  readonly attribute SVGAnimatedEnumeration lengthAdjust;

  long getNumberOfChars();
  float getComputedTextLength();
  float getSubStringLength(in unsigned long charnum, in unsigned long nchars) raises(DOMException);
  SVGPoint getStartPositionOfChar(in unsigned long charnum) raises(DOMException);
  SVGPoint getEndPositionOfChar(in unsigned long charnum) raises(DOMException);
  SVGRect getExtentOfChar(in unsigned long charnum) raises(DOMException);
  float getRotationOfChar(in unsigned long charnum) raises(DOMException);
  long getCharNumAtPosition(in SVGPoint point);
  void selectSubString(in unsigned long charnum, in unsigned long nchars) raises(DOMException);
#endif
