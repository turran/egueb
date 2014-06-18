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

EAPI void egueb_dom_init(void);
EAPI void egueb_dom_shutdown(void);

/* Errors (Exceptions) */
EAPI extern Eina_Error EGUEB_DOM_ERROR_INDEX_SIZE;
EAPI extern Eina_Error EGUEB_DOM_ERROR_DOMSTRING_SIZE;
EAPI extern Eina_Error EGUEB_DOM_ERROR_HIERARCHY_REQUEST;
EAPI extern Eina_Error EGUEB_DOM_ERROR_WRONG_DOCUMENT;
EAPI extern Eina_Error EGUEB_DOM_ERROR_INVALID_CHARACTER;
EAPI extern Eina_Error EGUEB_DOM_ERROR_NO_DATA_ALLOWED;
EAPI extern Eina_Error EGUEB_DOM_ERROR_NO_MODIFICATION_ALLOWED;
EAPI extern Eina_Error EGUEB_DOM_ERROR_NOT_FOUND;
EAPI extern Eina_Error EGUEB_DOM_ERROR_NOT_SUPPORTED;
EAPI extern Eina_Error EGUEB_DOM_ERROR_INUSE_ATTRIBUTE;
EAPI extern Eina_Error EGUEB_DOM_ERROR_INVALID_STATE;
EAPI extern Eina_Error EGUEB_DOM_ERROR_SYNTAX;
EAPI extern Eina_Error EGUEB_DOM_ERROR_INVALID_MODIFICATION;
EAPI extern Eina_Error EGUEB_DOM_ERROR_NAMESPACE;
EAPI extern Eina_Error EGUEB_DOM_ERROR_INVALID_ACCESS;
EAPI extern Eina_Error EGUEB_DOM_ERROR_VALIDATION;
EAPI extern Eina_Error EGUEB_DOM_ERROR_TYPE_MISMATCH;

/* Shared strings */
EAPI extern Egueb_Dom_String *EGUEB_DOM_XLINK_HREF;
EAPI extern Egueb_Dom_String *EGUEB_DOM_NAME_SCRIPT;

#endif
