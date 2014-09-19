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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eina.h>

#include "Egueb_Css.h"
#include "egueb_css_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

static int egueb_css_init_count = 0;

static void _egueb_css_strings_init(void)
{
	EGUEB_CSS_NAME_STYLE = egueb_dom_string_new_with_string("style");
	EGUEB_CSS_NAME_FONT = egueb_dom_string_new_with_string("font");
	EGUEB_CSS_NAME_FONT_FAMILY = egueb_dom_string_new_with_string("font-family");
	EGUEB_CSS_NAME_FONT_SIZE = egueb_dom_string_new_with_string("font-size");
	EGUEB_CSS_NAME_FONT_STYLE = egueb_dom_string_new_with_string("font-style");
	EGUEB_CSS_NAME_FONT_VARIANT = egueb_dom_string_new_with_string("font-variant");
	EGUEB_CSS_NAME_FONT_WEIGHT = egueb_dom_string_new_with_string("font-weight");
}

static void _egueb_css_strings_shutdown(void)
{
	egueb_dom_string_unref(EGUEB_CSS_NAME_FONT);
	egueb_dom_string_unref(EGUEB_CSS_NAME_FONT_FAMILY);
	egueb_dom_string_unref(EGUEB_CSS_NAME_FONT_SIZE);
	egueb_dom_string_unref(EGUEB_CSS_NAME_FONT_STYLE);
	egueb_dom_string_unref(EGUEB_CSS_NAME_FONT_VARIANT);
	egueb_dom_string_unref(EGUEB_CSS_NAME_FONT_WEIGHT);
	egueb_dom_string_unref(EGUEB_CSS_NAME_STYLE);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

int _egueb_css_log_dom_global = -1;

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_CSS_NAME_STYLE;
Egueb_Dom_String *EGUEB_CSS_NAME_FONT;
Egueb_Dom_String *EGUEB_CSS_NAME_FONT_FAMILY;
Egueb_Dom_String *EGUEB_CSS_NAME_FONT_SIZE;
Egueb_Dom_String *EGUEB_CSS_NAME_FONT_STYLE;
Egueb_Dom_String *EGUEB_CSS_NAME_FONT_VARIANT;
Egueb_Dom_String *EGUEB_CSS_NAME_FONT_WEIGHT;

EAPI int egueb_css_init(void)
{
	if (++egueb_css_init_count != 1)
		return egueb_css_init_count;

	if (!eina_init())
	{
		fprintf(stderr, "Egueb_Css: Eina init failed");
		return --egueb_css_init_count;
	}

	_egueb_css_log_dom_global = eina_log_domain_register("egueb_css", EGUEB_CSS_DEFAULT_LOG_COLOR);
	if (_egueb_css_log_dom_global < 0)
	{
		EINA_LOG_ERR("Egueb_Css Can not create a general log domain.");
		goto shutdown_eina;
	}
	_egueb_css_strings_init();

	return egueb_css_init_count;

shutdown_eina:
	eina_shutdown();
	return --egueb_css_init_count;
}

EAPI int egueb_css_shutdown(void)
{
	if (--egueb_css_init_count != 0)
		return egueb_css_init_count;

	_egueb_css_strings_shutdown();
	eina_log_domain_unregister(_egueb_css_log_dom_global);
	_egueb_css_log_dom_global = -1;
	eina_shutdown();

	return egueb_css_init_count;
}
