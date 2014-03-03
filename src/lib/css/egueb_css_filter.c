/* Egueb_Css - CSS
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
#include "egueb_css_selector.h"
#include "egueb_css_rule.h"
#include "egueb_css_style.h"
#include "egueb_css_context.h"

#include "egueb_css_filter_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Css_Filter
{
	Egueb_Css_Filter_Descriptor descriptor;
	void *data;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_css_filter_test(Egueb_Css_Filter *f, Egueb_Css_Context *c, void *e)
{
	if (f->descriptor.test)
		return f->descriptor.test(f->data, c, e);
	return EINA_FALSE;
}

void egueb_css_filter_delete(Egueb_Css_Filter *f)
{
	if (f->descriptor.free)
		f->descriptor.free(f->data);
	free(f);
}

Egueb_Css_Filter * egueb_css_filter_new(Egueb_Css_Filter_Descriptor *d, void *data)
{
	Egueb_Css_Filter *f;

	f = calloc(1, sizeof(Egueb_Css_Filter));
	f->data = data;
	f->descriptor = *d;

	return f;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
