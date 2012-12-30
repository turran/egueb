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
#include "esvg_private_element.h"
#include "esvg_private_attribute_animation.h"
#include "esvg_private_animation.h"
#include "esvg_private_animate_base.h"
#include "esvg_private_animate.h"
#include "esvg_private_svg.h"

#include "esvg_element_animate.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_element_animate_log

static int _esvg_element_animate_log = -1;

typedef struct _Esvg_Element_Animate
{
	/* properties */
	/* interface */
	/* private */
} Esvg_Element_Animate;

static Esvg_Element_Animate * _esvg_element_animate_get(Edom_Tag *t)
{
	Esvg_Element_Animate *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TYPE_ANIMATE)
		return NULL;
	thiz = esvg_animate_base_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         The Animate Base interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_element_animate_free(Edom_Tag *t)
{
	Esvg_Element_Animate *thiz;

	thiz = _esvg_element_animate_get(t);
	free(thiz);
}

static Eina_Bool _esvg_element_animate_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	return EINA_FALSE;
}

static Eina_Bool _esvg_element_animate_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Eina_Bool _esvg_element_animate_type_descriptor_get(Edom_Tag *t,
		const char *name, Esvg_Attribute_Animated_Descriptor **d)
{
	if (!strcmp(name, "esvg_animated_transform"))
		goto done;
	*d = esvg_attribute_animated_descriptor_get(name);
	if (*d) return EINA_TRUE;
done:
	ERR("Can not animate '%s'", name);
	return EINA_FALSE;
}

static Esvg_Animate_Base_Descriptor _descriptor = {
	/* .attribute_get 	= */ _esvg_element_animate_attribute_get,
	/* .free 		= */ _esvg_element_animate_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_element_animate_attribute_set,
	/* .type_descriptor_get	= */ _esvg_element_animate_type_descriptor_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_animate_new(void)
{
	Esvg_Element_Animate *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Element_Animate));

	t = esvg_animate_base_new(&_descriptor, ESVG_TYPE_ANIMATE, thiz);
	return t;
}
/* The ender wrapper */
#define _esvg_element_animate_delete NULL
#include "generated/esvg_generated_element_animate.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_animate_init(void)
{
	_esvg_element_animate_log = eina_log_domain_register("esvg_element_animate", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_animate_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_animate_init();
}

void esvg_element_animate_shutdown(void)
{
	if (_esvg_element_animate_log < 0)
		return;
	_esvg_element_animate_shutdown();
	eina_log_domain_unregister(_esvg_element_animate_log);
	_esvg_element_animate_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_element_animate_new(void)
{
	return ESVG_ELEMENT_NEW("SVGAnimateElement");
}

