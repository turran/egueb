/* Egueb_Dom - DOM
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#ifndef _EGUEB_DOM_H
#define _EGUEB_DOM_H

#include <Eina.h>
#include <Enesim.h>
#include <Etch.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EGUEB_DOM_BUILD
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Egueb_Dom_Group DOM
 * @brief DOM related API
 * @{
 */

#include "egueb_dom_build.h"
#include "egueb_dom_string.h"
#include "egueb_dom_main.h"
#include "egueb_dom_list.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_node.h"
#include "egueb_dom_element.h"
#include "egueb_dom_document.h"
#include "egueb_dom_parser.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_external.h"
#include "egueb_dom_event_mouse.h"
#include "egueb_dom_event_mutation.h"
#include "egueb_dom_event_ui.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_attr_string.h"
#include "egueb_dom_attr_string_list.h"
#include "egueb_dom_character_data.h"
#include "egueb_dom_input.h"
#include "egueb_dom_text.h"
#include "egueb_dom_uri.h"
#include "egueb_dom_utils.h"
#include "egueb_dom_value.h"
#include "egueb_dom_value_dom_string.h"
#include "egueb_dom_value_int.h"
#include "egueb_dom_string_list.h"

#include "egueb_dom_element_external.h"
#include "egueb_dom_document_external.h"

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /*_EGUEB_DOM_H*/
