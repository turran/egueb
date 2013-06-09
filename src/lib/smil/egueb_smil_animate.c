/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
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
#include "egueb_smil_main_private.h"
#include "egueb_smil_private_attribute_presentation.h"
#include "egueb_smil_context_private.h"
#include "egueb_smil_element_private.h"
#include "egueb_smil_private_attribute_animation.h"
#include "egueb_smil_element_animation_private.h"
#include "egueb_smil_animate_base_private.h"
#include "egueb_smil_element_animate_private.h"
#include "egueb_smil_element_svg_private.h"

#include "egueb_smil_element_animate.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SMIL_LOG_DEFAULT _egueb_smil_element_animate_log

static int _egueb_smil_element_animate_log = -1;

typedef struct _Egueb_Smil_Element_Animate
{
	/* properties */
	/* interface */
	/* private */
} Egueb_Smil_Element_Animate;

static Egueb_Smil_Element_Animate * _egueb_smil_element_animate_get(Egueb_Dom_Tag *t)
{
	Egueb_Smil_Element_Animate *thiz;

	if (egueb_smil_element_internal_type_get(t) != EGUEB_SMIL_TYPE_ANIMATE)
		return NULL;
	thiz = egueb_smil_animate_base_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         The Animate Base interface                         *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_element_animate_free(Egueb_Dom_Tag *t)
{
	Egueb_Smil_Element_Animate *thiz;

	thiz = _egueb_smil_element_animate_get(t);
	free(thiz);
}

static Eina_Bool _egueb_smil_element_animate_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	return EINA_FALSE;
}

static Eina_Bool _egueb_smil_element_animate_attribute_get(Egueb_Dom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Eina_Bool _egueb_smil_element_animate_type_descriptor_get(Egueb_Dom_Tag *t,
		const char *name, Egueb_Smil_Attribute_Animated_Descriptor **d)
{
	if (!strcmp(name, "SVGAnimatedTransform"))
		goto done;
	*d = egueb_smil_attribute_animated_descriptor_get(name);
	if (*d) return EINA_TRUE;
done:
	ERR("Can not animate '%s'", name);
	return EINA_FALSE;
}

static Egueb_Smil_Animate_Base_Descriptor _descriptor = {
	/* .attribute_get 	= */ _egueb_smil_element_animate_attribute_get,
	/* .free 		= */ _egueb_smil_element_animate_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _egueb_smil_element_animate_attribute_set,
	/* .type_descriptor_get	= */ _egueb_smil_element_animate_type_descriptor_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_smil_element_animate_new(void)
{
	Egueb_Smil_Element_Animate *thiz;
	Egueb_Dom_Tag *t;

	thiz = calloc(1, sizeof(Egueb_Smil_Element_Animate));
	t = egueb_smil_animate_base_new(&_descriptor, EGUEB_SMIL_TYPE_ANIMATE, thiz);
	return t;
}
/* The ender wrapper */
#define _egueb_smil_element_animate_delete NULL
#include "egueb_smil_generated_element_animate.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_smil_element_animate_init(void)
{
	_egueb_smil_element_animate_log = eina_log_domain_register("egueb_smil_element_animate", EGUEB_SMIL_LOG_COLOR_DEFAULT);
	if (_egueb_smil_element_animate_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_smil_element_animate_init();
}

void egueb_smil_element_animate_shutdown(void)
{
	if (_egueb_smil_element_animate_log < 0)
		return;
	_egueb_smil_element_animate_shutdown();
	eina_log_domain_unregister(_egueb_smil_element_animate_log);
	_egueb_smil_element_animate_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * egueb_smil_element_animate_new(void)
{
	return EGUEB_SMIL_ELEMENT_NEW("SVGAnimateElement");
}

