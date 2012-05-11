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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _run = 1;

typedef struct _Esvg_Context_Setup
{
	Edom_Tag *t;
} Esvg_Context_Setup;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_context_init(Esvg_Context *thiz)
{
	thiz->run = _run++;
	thiz->queue = NULL;
}

void esvg_context_setup_enqueue(Esvg_Context *thiz, Edom_Tag *t)
{

}

void esvg_context_setup_dequeue(Esvg_Context *thiz)
{
	/* for every context setup dequeue it */
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

