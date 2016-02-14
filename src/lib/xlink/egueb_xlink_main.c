/* Egueb_Xlink - XLINK
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
#include "egueb_xlink_private.h"

#include "egueb_xlink_main.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _egueb_xlink_init_count = 0;

static void _egueb_xlink_strings_init(void)
{
	EGUEB_XLINK_NAME_NS = egueb_dom_string_new_with_chars("http://www.w3.org/1999/xlink");
	EGUEB_XLINK_NAME_HREF = egueb_dom_string_new_with_chars("href");
}

static void _egueb_xlink_strings_shutdown(void)
{
	egueb_dom_string_unref(EGUEB_XLINK_NAME_NS);
	egueb_dom_string_unref(EGUEB_XLINK_NAME_HREF);
}

static Eina_Bool _egueb_xlink_dependencies_init(void)
{
	if (!eina_init())
	{
		fprintf(stderr, "Egueb_Xlink: Eina init failed");
		return EINA_FALSE;
	}

	if (!enesim_init())
	{
		fprintf(stderr, "Egueb_Xlink: Enesim init failed");
		goto shutdown_eina;
	}

	egueb_dom_init();
	return EINA_TRUE;

shutdown_eina:
	eina_shutdown();
	return EINA_FALSE;

}

static void _egueb_xlink_dependencies_shutdown(void)
{
	egueb_dom_shutdown();
	enesim_shutdown();
	eina_shutdown();
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int egueb_xlink_log_dom_global = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_XLINK_NAME_NS;
Egueb_Dom_String *EGUEB_XLINK_NAME_HREF;

EAPI int egueb_xlink_init(void)
{
	if (++_egueb_xlink_init_count != 1)
		return _egueb_xlink_init_count;

	if (!_egueb_xlink_dependencies_init())
		return --_egueb_xlink_init_count;

	egueb_xlink_log_dom_global = eina_log_domain_register("egueb_xlink", 0);
	if (egueb_xlink_log_dom_global < 0)
	{
		EINA_LOG_ERR("Egueb_Xlink: Can not create a general log domain.");
		_egueb_xlink_dependencies_shutdown();
		return --_egueb_xlink_init_count;
	}

	_egueb_xlink_strings_init();
	return egueb_xlink_log_dom_global;
}

EAPI int egueb_xlink_shutdown(void)
{
	if (--_egueb_xlink_init_count != 0)
		return _egueb_xlink_init_count;

	_egueb_xlink_strings_shutdown();
	eina_log_domain_unregister(egueb_xlink_log_dom_global);
        egueb_xlink_log_dom_global = -1;
	_egueb_xlink_dependencies_shutdown();
	return egueb_xlink_log_dom_global;
}

