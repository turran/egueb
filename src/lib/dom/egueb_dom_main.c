/* Egueb_Dom - DOM
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
#include "egueb_dom_private.h"

#include "egueb_dom_string.h"
#include "egueb_dom_main.h"

#include "egueb_dom_registry_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _init_count = 0;
static Eina_Bool _initializing;
static Eina_Bool _deinitializing;

static void _egueb_dom_strings_init(void)
{
	EGUEB_DOM_XLINK_HREF = egueb_dom_string_new_with_string("xlink:href");
}

static void _egueb_dom_strings_shutdown(void)
{
	egueb_dom_string_unref(EGUEB_DOM_XLINK_HREF);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int egueb_dom_log_dom_global = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Error EGUEB_DOM_ERROR_INDEX_SIZE;
EAPI Eina_Error EGUEB_DOM_ERROR_DOMSTRING_SIZE;
EAPI Eina_Error EGUEB_DOM_ERROR_HIERARCHY_REQUEST;
EAPI Eina_Error EGUEB_DOM_ERROR_WRONG_DOCUMENT;
EAPI Eina_Error EGUEB_DOM_ERROR_INVALID_CHARACTER;
EAPI Eina_Error EGUEB_DOM_ERROR_NO_DATA_ALLOWED;
EAPI Eina_Error EGUEB_DOM_ERROR_NO_MODIFICATION_ALLOWED;
EAPI Eina_Error EGUEB_DOM_ERROR_NOT_FOUND;
EAPI Eina_Error EGUEB_DOM_ERROR_NOT_SUPPORTED;
EAPI Eina_Error EGUEB_DOM_ERROR_INUSE_ATTRIBUTE;
EAPI Eina_Error EGUEB_DOM_ERROR_INVALID_STATE;
EAPI Eina_Error EGUEB_DOM_ERROR_SYNTAX;
EAPI Eina_Error EGUEB_DOM_ERROR_INVALID_MODIFICATION;
EAPI Eina_Error EGUEB_DOM_ERROR_NAMESPACE;
EAPI Eina_Error EGUEB_DOM_ERROR_INVALID_ACCESS;
EAPI Eina_Error EGUEB_DOM_ERROR_VALIDATION;
EAPI Eina_Error EGUEB_DOM_ERROR_TYPE_MISMATCH;

Egueb_Dom_String *EGUEB_DOM_XLINK_HREF;

EAPI void egueb_dom_init(void)
{
	if (_initializing) return;

	_initializing = EINA_TRUE;
	if (_init_count) goto done;
	eina_init();
	enesim_init();
	egueb_dom_log_dom_global = eina_log_domain_register("egueb_dom", 0);
	if (egueb_dom_log_dom_global < 0)
	{
		EINA_LOG_ERR("Egueb_Dom: Can not create a general log domain.");
		goto shutdown_eina;
	}

	/* register the dom errors */
	EGUEB_DOM_ERROR_DOMSTRING_SIZE = eina_error_msg_register("The specified range of text does not fit into a DOMString");
	EGUEB_DOM_ERROR_HIERARCHY_REQUEST = eina_error_msg_register("A Node is inserted somewhere it doesn't belong");
	EGUEB_DOM_ERROR_INDEX_SIZE = eina_error_msg_register("Index or size is negative, or greater than the allowed value");
	EGUEB_DOM_ERROR_INUSE_ATTRIBUTE = eina_error_msg_register("An attempt is made to add an attribute that is already in use elsewhere");
	EGUEB_DOM_ERROR_INVALID_ACCESS = eina_error_msg_register("A parameter or an operation is not supported by the underlying object");
	EGUEB_DOM_ERROR_INVALID_CHARACTER = eina_error_msg_register("An invalid or illegal character is specified, such as in an XML name");
	EGUEB_DOM_ERROR_INVALID_STATE = eina_error_msg_register("An attempt is made to use an object that is not, or is no longer, usable");
	EGUEB_DOM_ERROR_NAMESPACE = eina_error_msg_register("An attempt is made to create or change an object in a way which is incorrect with regard to namespaces");
	EGUEB_DOM_ERROR_NOT_FOUND = eina_error_msg_register("An attempt is made to reference a Node in a context where it does not exist");
	EGUEB_DOM_ERROR_NOT_SUPPORTED = eina_error_msg_register("The implementation does not support the requested type of object or operation");
	EGUEB_DOM_ERROR_NO_DATA_ALLOWED = eina_error_msg_register("Data is specified for a Node which does not support data");
	EGUEB_DOM_ERROR_NO_MODIFICATION_ALLOWED = eina_error_msg_register("");
	EGUEB_DOM_ERROR_SYNTAX = eina_error_msg_register("");
	EGUEB_DOM_ERROR_INVALID_MODIFICATION = eina_error_msg_register("");
	EGUEB_DOM_ERROR_VALIDATION = eina_error_msg_register("");
	EGUEB_DOM_ERROR_TYPE_MISMATCH = eina_error_msg_register("");
	EGUEB_DOM_ERROR_WRONG_DOCUMENT = eina_error_msg_register("Node is used in a different document than the one that created it (that doesn't support it)");

	_egueb_dom_strings_init();
	egueb_dom_registry_init();
done:
	_init_count++;
	_initializing = EINA_FALSE;
	return;

shutdown_eina:
	eina_shutdown();
}

EAPI void egueb_dom_shutdown(void)
{
	if (_deinitializing)
		return;

	_deinitializing = EINA_TRUE;
	if (_init_count != 1) goto done;
	egueb_dom_registry_shutdown();
	_egueb_dom_strings_shutdown();
	eina_log_domain_unregister(egueb_dom_log_dom_global);
        egueb_dom_log_dom_global = -1;
	enesim_shutdown();
	eina_shutdown();
done:
	_init_count--;
	_deinitializing = EINA_FALSE;
}
