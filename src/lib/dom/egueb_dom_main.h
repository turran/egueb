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

#ifndef _EGUEB_DOM_MAIN_H_
#define _EGUEB_DOM_MAIN_H_

EAPI void edom_init(void);
EAPI void edom_shutdown(void);

/* Errors (Exceptions) */
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

#endif
