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
static Eina_Bool _id_test(void *data, Ecss_Context *c, Ecss_Element *e)
{
	char *id_name = data;
	const char *id_data;

	/* check if the id attribute is equal to the id set */
	id_data = c->property_get(e, "id");
	if (!id_data) return EINA_FALSE;

	if (!strcmp(id_data, id_name))
		return EINA_TRUE;
	return EINA_FALSE;
}

static void _id_free(void *data)
{
	free(data);
}

static Ecss_Filter_Descriptor _descriptor = {
	/* .test	= */ _id_test,
	/* .free	= */ _id_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ecss_Filter * ecss_filter_id_new(const char *name)
{
	Ecss_Filter *f;

	if (!name) return NULL;

	ecss_filter_new(&_descriptor, strdup(name));
}

