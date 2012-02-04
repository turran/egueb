/* Ecss - CSS
 * Copyright (C) 2011 Jorge Luis Zapata
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
#ifndef _ECSS_H
#define _ECSS_H

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef ECSS_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EGUEB_BUILD */
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
 * @mainpage Ecss
 * @section intro Introduction
 * @section dependencies Dependencies
 * - Eina
 *
 * @todo
 */

typedef struct _Ecss_Selector Ecss_Selector;
typedef struct _Ecss_Rule Ecss_Rule;
typedef struct _Ecss_Style Ecss_Style;
typedef void Ecss_Element;
typedef struct _Ecss_Filter Ecss_Filter;

typedef enum _Ecss_Combinator
{
	ECSS_NONE,
	ECSS_DESCENDANT,
	ECSS_CHILD,
	ECSS_ADJACENT_SIBLING,
	ECSS_SIBLING,
} Ecss_Combinator;

typedef void (*Ecss_Context_Property_Set)(Ecss_Element *e, const char *property, const char *value);
typedef const char * (*Ecss_Context_Property_Get)(Ecss_Element *e, const char *property);
typedef Ecss_Element * (*Ecss_Context_Get_Child)(Ecss_Element *e);
typedef Ecss_Element * (*Ecss_Context_Get_Next_Sibling)(Ecss_Element *e);
typedef const char * (*Ecss_Context_Get_Name)(Ecss_Element *e);

typedef struct _Ecss_Context
{
	Ecss_Context_Property_Set property_set;
	Ecss_Context_Property_Get property_get;
	Ecss_Context_Get_Name get_name;
	Ecss_Context_Get_Child get_child;
	Ecss_Context_Get_Next_Sibling get_next_sibling;
} Ecss_Context;

EAPI int ecss_init(void);
EAPI int ecss_shutdown(void);

EAPI Ecss_Selector * ecss_selector_new(void);
EAPI void ecss_selector_subject_set(Ecss_Selector *thiz, const char *subject);
EAPI void ecss_selector_filter_add(Ecss_Selector *thiz, Ecss_Filter *f);
EAPI void ecss_selector_combinator_set(Ecss_Selector *thiz, Ecss_Selector *ss, Ecss_Combinator c);

EAPI Ecss_Rule * ecss_rule_new(Ecss_Selector *s);
EAPI void ecss_rule_declaration_add(Ecss_Rule *thiz, const char *property,
		const char *value);

EAPI Ecss_Style * ecss_style_new(void);
EAPI Ecss_Style * ecss_style_load_from_file(const char *file);
EAPI Ecss_Style * ecss_style_load_from_content(const char *content, size_t len);
EAPI void ecss_style_rule_add(Ecss_Style *thiz, Ecss_Rule *r);

EAPI void ecss_context_style_apply(Ecss_Context *c, Ecss_Style *s, Ecss_Element *e);

#endif /*_ECSS_H*/
