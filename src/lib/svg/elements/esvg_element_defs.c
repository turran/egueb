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

#include "esvg_element_defs.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Element_Defs
{
	/* properties */
	/* interface */
	/* private */
} Esvg_Element_Defs;

static Esvg_Element_Defs * _esvg_element_defs_get(Edom_Tag *t)
{
	Esvg_Element_Defs *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TYPE_DEFS)
		return NULL;
	thiz = esvg_element_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_element_defs_free(Edom_Tag *t)
{
	Esvg_Element_Defs *thiz;

	thiz = _esvg_element_defs_get(t);
	free(thiz);
}

static Esvg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ NULL,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _esvg_element_defs_free,
	/* .attribute_set 	= */ NULL,
	/* .initialize 		= */ NULL,
	/* .setup		= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_defs_new(void)
{
	Esvg_Element_Defs *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Element_Defs));
	if (!thiz) return NULL;

	t = esvg_element_new(&_descriptor, ESVG_TYPE_DEFS, thiz);
	return t;
}

/* The ender wrapper */
#define _esvg_element_defs_delete NULL
#include "esvg_generated_element_defs.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_defs_init(void)
{
	_esvg_element_defs_init();
}

void esvg_element_defs_shutdown(void)
{
	_esvg_element_defs_shutdown();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_element_defs_new(void)
{
	return ESVG_ELEMENT_NEW("SVGDefsElement");
}
