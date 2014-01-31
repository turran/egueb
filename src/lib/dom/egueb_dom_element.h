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

#ifndef _EGUEB_DOM_ELEMENT_H_
#define _EGUEB_DOM_ELEMENT_H_

#include "egueb_dom_string.h"
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"

/**
 * @defgroup Egueb_Dom_Element_Group Element
 * @brief Element
 * @ingroup Egueb_Dom_Group
 * @{
 */

#ifdef EINA_ENABLE_LOG
#ifdef EINA_LOG_LEVEL_MAXIMUM
#define EGUEB_DOM_ELEMENT_LOG(n, DOM, LEVEL, fmt, ...)				\
	do {									\
		if (LEVEL <= EINA_LOG_LEVEL_MAXIMUM) {				\
			Egueb_Dom_String *s;					\
			s = egueb_dom_element_name_get(EGUEB_DOM_NODE(n));	\
			eina_log_print(DOM, LEVEL, __FILE__, __FUNCTION__,	\
					 __LINE__, "<%s> " fmt,			\
					egueb_dom_string_string_get(s), 	\
					## __VA_ARGS__);			\
			egueb_dom_string_unref(s);				\
		}								\
	} while (0)
#else /* EINA_LOG_LEVEL_MAXIMUM */
#define EGUEB_DOM_ELEMENT_LOG(n, DOM, LEVEL, fmt, ...)				\
	do {									\
		Egueb_Dom_String *s;						\
		s = egueb_dom_element_name_get(EGUEB_DOM_NODE(n));		\
		eina_log_print(DOM, LEVEL, __FILE__, __FUNCTION__, __LINE__,	\
				"<%s> " fmt, egueb_dom_string_string_get(s),	\
				## __VA_ARGS__);				\
		egueb_dom_string_unref(s);					\
	} while (0)
#endif
#else
#define EGUEB_DOM_ELEMENT_LOG(n, DOM, LEVEL, fmt, ...)          \
	do { (void) n, (void) DOM; (void) LEVEL; (void) fmt; } while (0)
#endif

#define EGUEB_DOM_ELEMENT_LOG_DOM_CRIT(n, DOM, fmt, ...) \
  EGUEB_DOM_ELEMENT_LOG(n, DOM, EGUEB_DOM_ELEMENT_LOG_LEVEL_CRITICAL, fmt, ## __VA_ARGS__)

#define EGUEB_DOM_ELEMENT_LOG_DOM_ERR(n, DOM, fmt, ...) \
  EGUEB_DOM_ELEMENT_LOG(n, DOM, EINA_LOG_LEVEL_ERR, fmt, ## __VA_ARGS__)

#define EGUEB_DOM_ELEMENT_LOG_DOM_INFO(n, DOM, fmt, ...) \
  EGUEB_DOM_ELEMENT_LOG(n, DOM, EINA_LOG_LEVEL_INFO, fmt, ## __VA_ARGS__)

#define EGUEB_DOM_ELEMENT_LOG_DOM_DBG(n, DOM, fmt, ...) \
  EGUEB_DOM_ELEMENT_LOG(n, DOM, EINA_LOG_LEVEL_DBG, fmt, ## __VA_ARGS__)

#define EGUEB_DOM_ELEMENT_LOG_DOM_WARN(n, DOM, fmt, ...) \
  EGUEB_DOM_ELEMENT_LOG(n, DOM, EINA_LOG_LEVEL_WARN, fmt, ## __VA_ARGS__)

EAPI Enesim_Object_Descriptor * egueb_dom_element_descriptor_get(void);

EAPI Egueb_Dom_String * egueb_dom_element_name_get(Egueb_Dom_Node *node);
EAPI Egueb_Dom_String * egueb_dom_element_tag_name_get(Egueb_Dom_Node *node);

EAPI Eina_Bool egueb_dom_element_attribute_add(Egueb_Dom_Node *n,
		Egueb_Dom_Node *attr, Eina_Error *err);
EAPI Egueb_Dom_String * egueb_dom_element_attribute_get(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name);
EAPI Eina_Bool egueb_dom_element_attribute_set(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_String *value,
		Eina_Error *err);
EAPI Eina_Bool egueb_dom_element_attribute_type_set(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_String *value, Eina_Error *err);

/* TODO rename every element_property_foo function */
EAPI Eina_Bool egueb_dom_element_property_set(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, ...);
EAPI Eina_Bool egueb_dom_element_property_get(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, ...);

EAPI Eina_Bool egueb_dom_element_property_set_va(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, va_list args);
EAPI Eina_Bool egueb_dom_element_property_get_va(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, va_list args);

EAPI Eina_Bool egueb_dom_element_property_value_set(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_Value *v, Eina_Error *err);
EAPI Eina_Bool egueb_dom_element_property_value_get(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_Value *v, Eina_Error *err);

EAPI Egueb_Dom_Node * egueb_dom_element_property_fetch(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name);

EAPI Eina_Bool egueb_dom_element_changed(Egueb_Dom_Node *n);
EAPI void egueb_dom_element_request_process(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_dom_element_is_enqueued(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_dom_element_process(Egueb_Dom_Node *n);

/**
 * @}
 */

#endif
