/* Esvg - SVG
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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_element_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _run = 1;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_context_init(Esvg_Context *thiz)
{
	thiz->run = _run++;
	thiz->queue = NULL;
}

void esvg_context_shutdown(Esvg_Context *thiz)
{
	eina_list_free(thiz->queue);
}

void esvg_context_setup_enqueue(Esvg_Context *thiz, Edom_Tag *t)
{
	printf("enqueuing tag %p\n", t);
	/* FIXME it might be possible that an element is enqueued
	 * when we are dequeing the context, we need to handle
	 * that, but for now */
	if (thiz->dequeuing)
	{
		printf("enqueuing while dequeuing\n");
	}
	thiz->queue = eina_list_append(thiz->queue, t);
}

void esvg_context_setup_dequeue(Esvg_Context *thiz)
{
	Eina_List *l;
	Edom_Tag *t;

	/* FIXME it might be possible that the element enqueue again? */
	EINA_LIST_FOREACH(thiz->queue, l, t)
	{
		printf("dequeing tag %p\n", t);
		/* FIXME for every tag, call the setup */
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

