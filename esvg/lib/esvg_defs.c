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
typedef struct _Esvg_Defs
{
	/* properties */
	/* interface */
	/* private */
} Esvg_Defs;

static Esvg_Defs * _esvg_defs_get(Edom_Tag *t)
{
	Esvg_Defs *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_DEFS)
		return NULL;
	thiz = esvg_element_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_defs_free(Edom_Tag *t)
{
	Esvg_Defs *thiz;

	thiz = _esvg_defs_get(t);
	free(thiz);
}

static Esvg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ NULL,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_defs_free,
	/* .attribute_set 	= */ NULL,
	/* .initialize 		= */ NULL,
	/* .clone		= */ NULL,
	/* .setup		= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_defs_new(void)
{
	Esvg_Defs *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Defs));
	if (!thiz) return NULL;

	t = esvg_element_new(&_descriptor, ESVG_DEFS, thiz);
	return t;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_defs.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_defs_new(void)
{
	return ender_element_new_with_namespace("defs", "esvg");
}
