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
#include "egueb_smil_private.h"

#include "egueb_smil_main.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _init_count = 0;

/* our helpful strings */
static void _egueb_smil_strings_init(void)
{
	EGUEB_SMIL_NAME_SET = egueb_dom_string_new_with_string("set");
	EGUEB_SMIL_NAME_ANIMATE = egueb_dom_string_new_with_string("animate");

	EGUEB_SMIL_ADDITIVE = egueb_dom_string_new_with_string("additive");
	EGUEB_SMIL_NAME_ACCUMULATE = egueb_dom_string_new_with_string("accumulate");
	EGUEB_SMIL_ATTRIBUTE_NAME = egueb_dom_string_new_with_string("attributeName");
	EGUEB_SMIL_BEGIN = egueb_dom_string_new_with_string("begin");
	EGUEB_SMIL_DUR = egueb_dom_string_new_with_string("dur");
	EGUEB_SMIL_END = egueb_dom_string_new_with_string("end");
	EGUEB_SMIL_FILL = egueb_dom_string_new_with_string("fill");
	EGUEB_SMIL_TO = egueb_dom_string_new_with_string("to");
	EGUEB_SMIL_FROM = egueb_dom_string_new_with_string("from");
	EGUEB_SMIL_BY = egueb_dom_string_new_with_string("by");
	EGUEB_SMIL_VALUES = egueb_dom_string_new_with_string("values");
	EGUEB_SMIL_REPEAT_COUNT = egueb_dom_string_new_with_string("repeatCount");
	EGUEB_SMIL_NAME_REPEAT_DUR = egueb_dom_string_new_with_string("repeatDur");
	EGUEB_SMIL_NAME_CALC_MODE = egueb_dom_string_new_with_string("calcMode");
	EGUEB_SMIL_NAME_KEY_TIMES = egueb_dom_string_new_with_string("keyTimes");
	EGUEB_SMIL_NAME_KEY_SPLINES = egueb_dom_string_new_with_string("keySplines");
}

static void _egueb_smil_strings_shutdown(void)
{
	egueb_dom_string_unref(EGUEB_SMIL_NAME_SET);
	egueb_dom_string_unref(EGUEB_SMIL_NAME_ANIMATE);

	egueb_dom_string_unref(EGUEB_SMIL_ADDITIVE);
	egueb_dom_string_unref(EGUEB_SMIL_NAME_ACCUMULATE);
	egueb_dom_string_unref(EGUEB_SMIL_ATTRIBUTE_NAME);
	egueb_dom_string_unref(EGUEB_SMIL_BEGIN);
	egueb_dom_string_unref(EGUEB_SMIL_DUR);
	egueb_dom_string_unref(EGUEB_SMIL_END);
	egueb_dom_string_unref(EGUEB_SMIL_FILL);
	egueb_dom_string_unref(EGUEB_SMIL_TO);
	egueb_dom_string_unref(EGUEB_SMIL_FROM);
	egueb_dom_string_unref(EGUEB_SMIL_BY);
	egueb_dom_string_unref(EGUEB_SMIL_VALUES);
	egueb_dom_string_unref(EGUEB_SMIL_REPEAT_COUNT);
	egueb_dom_string_unref(EGUEB_SMIL_NAME_REPEAT_DUR);
	egueb_dom_string_unref(EGUEB_SMIL_NAME_CALC_MODE);
	egueb_dom_string_unref(EGUEB_SMIL_NAME_KEY_TIMES);
	egueb_dom_string_unref(EGUEB_SMIL_NAME_KEY_SPLINES);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int egueb_smil_log_global = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_SMIL_NAME_SET;
Egueb_Dom_String *EGUEB_SMIL_NAME_ANIMATE;

Egueb_Dom_String *EGUEB_SMIL_ADDITIVE;
Egueb_Dom_String *EGUEB_SMIL_NAME_ACCUMULATE;
Egueb_Dom_String *EGUEB_SMIL_ATTRIBUTE_NAME;
Egueb_Dom_String *EGUEB_SMIL_BEGIN;
Egueb_Dom_String *EGUEB_SMIL_DUR;
Egueb_Dom_String *EGUEB_SMIL_END;
Egueb_Dom_String *EGUEB_SMIL_FILL;
Egueb_Dom_String *EGUEB_SMIL_TO;
Egueb_Dom_String *EGUEB_SMIL_FROM;
Egueb_Dom_String *EGUEB_SMIL_BY;
Egueb_Dom_String *EGUEB_SMIL_VALUES;
Egueb_Dom_String *EGUEB_SMIL_REPEAT_COUNT;
Egueb_Dom_String *EGUEB_SMIL_NAME_REPEAT_DUR;
Egueb_Dom_String *EGUEB_SMIL_NAME_CALC_MODE;
Egueb_Dom_String *EGUEB_SMIL_NAME_KEY_TIMES;
Egueb_Dom_String *EGUEB_SMIL_NAME_KEY_SPLINES;

EAPI void egueb_smil_init(void)
{
	if (_init_count) goto done;
	egueb_dom_init();
	egueb_xlink_init();

	egueb_smil_log_global = eina_log_domain_register("egueb_smil", 0);
	if (egueb_smil_log_global < 0)
	{
		EINA_LOG_ERR("Egueb_Smil: Can not create a general log domain.");
		goto shutdown_eina;
	}
	_egueb_smil_strings_init();
done:
	_init_count++;
	return;

shutdown_eina:
	egueb_dom_shutdown();
}

EAPI void egueb_smil_shutdown(void)
{
	if (_init_count != 1) goto done;
	_egueb_smil_strings_shutdown();
	eina_log_domain_unregister(egueb_smil_log_global);
        egueb_smil_log_global = -1;

	egueb_xlink_shutdown();
	egueb_dom_shutdown();
done:
	_init_count--;
}


