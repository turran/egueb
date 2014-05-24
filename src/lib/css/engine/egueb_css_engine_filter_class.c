/* Egueb_Css_Engine - CSS
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

#include "egueb_css_private.h"

#include "egueb_css_engine_selector_private.h"
#include "egueb_css_engine_rule_private.h"
#include "egueb_css_engine_style_private.h"
#include "egueb_css_engine_context_private.h"
#include "egueb_css_engine_filter_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _class_test(void *data, Egueb_Css_Engine_Context *c, void *e)
{
	char *class_name = data;
	const char *class_data;

	/* check if the class attribute is equal to the class set */
	class_data = c->attribute_get(e, "class");
	if (!class_data) return EINA_FALSE;

	/* the class can be one of the several classes */
	if (!strcmp(class_data, class_name))
		return EINA_TRUE;

	return EINA_FALSE;
}

static void _class_free(void *data)
{
	free(data);
}

static Egueb_Css_Engine_Filter_Descriptor _descriptor = {
	/* .test	= */ _class_test,
	/* .free	= */ _class_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Css_Engine_Filter * egueb_css_engine_filter_class_new(const char *name)
{
	char *n;

	if (!name) return NULL;

	n = strdup(name);
	if (!n) return NULL;

	return egueb_css_engine_filter_new(&_descriptor, n);
}
