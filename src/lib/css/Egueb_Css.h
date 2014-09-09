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
#ifndef _EGUEB_CSS_H
#define _EGUEB_CSS_H

#include <Eina.h>
#include <Egueb_Dom.h>

#include <stddef.h>
#include <stdint.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EGUEB_CSS_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

/**
 * @defgroup Egueb_Css_Group CSS
 * @brief CSS related API
 * @{
 */

/* core */
#include "egueb_css_main.h"
/* types */
#include "egueb_css_color.h"
#include "egueb_css_length.h"
#include "egueb_css_percentage.h"
#include "egueb_css_font_size.h"
#include "egueb_css_font_family.h"
#include "egueb_css_font_style.h"
#include "egueb_css_font_variant.h"
#include "egueb_css_font_weight.h"
#include "egueb_css_font.h"
/* values */
#include "egueb_css_value_color.h"
#include "egueb_css_value_font_family.h"
#include "egueb_css_value_font.h"
/* attributes */
#include "egueb_css_attr_font.h"
#include "egueb_css_attr_font_family.h"
#include "egueb_css_attr_style.h"
/* elements */
#include "egueb_css_element_style.h"

/** @} */

#endif /*_EGUEB_CSS_H*/
