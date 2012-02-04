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
struct _Ecss_Filter
{
	Ecss_Filter_Descriptor descriptor;
	void *data;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool ecss_filter_test(Ecss_Filter *f, Ecss_Context *c, Ecss_Element *e)
{
	if (f->descriptor.test)
		return f->descriptor.test(f->data, c, e);
	return EINA_FALSE;
}

void ecss_filter_delete(Ecss_Filter *f)
{
	if (f->descriptor.free)
		f->descriptor.free(f->data);
	free(f);
}

Ecss_Filter * ecss_filter_new(Ecss_Filter_Descriptor *d, void *data)
{
	Ecss_Filter *f;

	f = calloc(1, sizeof(Ecss_Filter));
	f->data = data;
	f->descriptor = *d;

	return f;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

