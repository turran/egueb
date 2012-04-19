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
#include "esvg_private_attribute_animation.h"
#include "esvg_private_element.h"
#include "esvg_private_animate.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Animate
{
	/* properties */
	/* interface */
	/* private */
} Esvg_Animate;

static Esvg_Animate * _esvg_animate_get(Edom_Tag *t)
{
	Esvg_Animate *thiz;

	if (esvg_element_type_get_internal(t) != ESVG_ANIMATE)
		return NULL;
	thiz = esvg_element_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_free(Edom_Tag *t)
{
	Esvg_Animate *thiz;

	thiz = _esvg_animate_get(t);
	free(thiz);
}

static Eina_Bool _esvg_animate_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	return EINA_TRUE;
}

static Eina_Bool _esvg_animate_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

/* TODO optimize so many 'ifs' */
static Eina_Bool _esvg_animate_setup(Edom_Tag *t,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Animate *thiz;

	thiz = _esvg_animate_get(t);
	return EINA_TRUE;
}

static Esvg_Element_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_animate_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_animate_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_animate_attribute_set,
	/* .clone		= */ _esvg_animate_clone,
	/* .setup		= */ _esvg_animate_setup,
	/* .renderer_get	= */ _esvg_animate_renderer_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_animate.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

