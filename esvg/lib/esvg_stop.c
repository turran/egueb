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
#include "esvg_private_element.h"
#include "esvg_private_stop.h"
#include "esvg_stop.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_STOP_OFFSET;

typedef struct _Esvg_Stop
{
	/* properties */
	Esvg_Length offset;
	/* interface */
	/* private */
} Esvg_Stop;

static Esvg_Stop * _esvg_stop_get(Edom_Tag *t)
{
	Esvg_Stop *thiz;

	if (esvg_element_type_get_internal(t) != ESVG_STOP)
		return NULL;
	thiz = esvg_element_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_stop_free(Edom_Tag *t)
{
	Esvg_Stop *thiz;

	thiz = _esvg_stop_get(t);
	free(thiz);
}

static Eina_Bool _esvg_stop_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "offset") == 0)
	{
		Esvg_Length offset;

		esvg_length_string_from(&offset, value, ESVG_LENGTH_0);
		printf("offset %g\n", offset.value);
		esvg_stop_offset_set(e, &offset);
	}
	/*
	 * stop color and stop opacity are part of the presentation attributes
	 * and already parsed on the element
	 */
	return EINA_TRUE;
}

static Eina_Bool _esvg_stop_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

/* TODO optimize so many 'ifs' */
static Eina_Bool _esvg_stop_setup(Edom_Tag *t,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Stop *thiz;

	thiz = _esvg_stop_get(t);
	printf("setting up the stop\n");
	return EINA_TRUE;
}

static Esvg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_stop_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_stop_free,
	/* .attribute_set 	= */ _esvg_stop_attribute_set,
	/* .initialize 		= */ NULL,
	/* .clone		= */ NULL,
	/* .setup		= */ _esvg_stop_setup,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_stop_new(void)
{
	Esvg_Stop *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Stop));
	if (!thiz) return NULL;

	/* default values */
	thiz->offset = ESVG_LENGTH_0;

	t = esvg_element_new(&_descriptor, ESVG_STOP, thiz);
	return t;
}

static void _esvg_stop_offset_set(Edom_Tag *t, Esvg_Length *offset)
{
	Esvg_Stop *thiz;

	thiz = _esvg_stop_get(t);
	if (!offset)
		return;
	thiz->offset = *offset;
}

static void _esvg_stop_offset_get(Edom_Tag *t, Esvg_Length *offset)
{
	Esvg_Stop *thiz;

	thiz = _esvg_stop_get(t);
	if (!offset)
		return;
	*offset = thiz->offset;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_stop.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_stop_new(void)
{
	return ender_element_new_with_namespace("stop", "esvg");
}

EAPI void esvg_stop_offset_set(Ender_Element *e, const Esvg_Length *offset)
{

}

