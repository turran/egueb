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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eina.h>

#include "Ecss.h"
#include "ecss_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _class_test(void *data, Ecss_Context *c, Ecss_Element *e)
{
	char *class_name = data;
	const char *class_data;

	/* check if the class attribute is equal to the class set */
	class_data = c->property_get(e, "class");
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

static Ecss_Filter_Descriptor _descriptor = {
	/* .test	= */ _class_test,
	/* .free	= */ _class_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ecss_Filter * ecss_filter_class_new(const char *name)
{
	Ecss_Filter *f;

	if (!name) return NULL;

	ecss_filter_new(&_descriptor, strdup(name));
}
