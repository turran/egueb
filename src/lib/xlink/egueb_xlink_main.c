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
static int _init_count = 0;

static void _egueb_dom_strings_init(void)
{
	EGUEB_XLINK_NAME_NS = egueb_dom_string_new_with_string("http://www.w3.org/1999/xlink");
	EGUEB_XLINK_NAME_HREF = egueb_dom_string_new_with_string("href");
}

static void _egueb_dom_strings_shutdown(void)
{
	egueb_dom_string_unref(EGUEB_XLINK_NAME_NS);
	egueb_dom_string_unref(EGUEB_XLINK_NAME_HREF);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int egueb_dom_log_dom_global = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_XLINK_NAME_NS;
Egueb_Dom_String *EGUEB_XLINK_NAME_XMLNS;

EAPI void egueb_xlink_init(void)
{
	if (_init_count) goto done;
	egueb_dom_init();

	_egueb_dom_strings_init();
done:
	_init_count++;
	return;
}

EAPI void egueb_xlink_shutdown(void)
{
	_deinitializing = EINA_TRUE;
	if (_init_count != 1) goto done;
	_egueb_dom_strings_shutdown();
done:
	_init_count--;
}

