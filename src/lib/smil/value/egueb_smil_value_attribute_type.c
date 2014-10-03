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
#include "egueb_smil_attribute_type.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Eina_Bool egueb_smil_attribute_type_string_from(
		Egueb_Smil_Attribute_Type *attribute_type, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "CSS"))
		*attribute_type = EGUEB_SMIL_ATTRIBUTE_TYPE_CSS;
	else if (!strcmp(attr, "XML"))
		*attribute_type = EGUEB_SMIL_ATTRIBUTE_TYPE_XML;
	else if (!strcmp(attr, "auto"))
		*attribute_type = EGUEB_SMIL_ATTRIBUTE_TYPE_AUTO;
	else
		ret = EINA_FALSE;
	return ret;
}

static char * egueb_smil_attribute_type_string_to(
		Egueb_Smil_Attribute_Type attribute_type)
{
	switch (attribute_type)
	{
		case EGUEB_SMIL_ATTRIBUTE_TYPE_CSS:
		return strdup("CSS");

		case EGUEB_SMIL_ATTRIBUTE_TYPE_XML:
		return strdup("XML");

		case EGUEB_SMIL_ATTRIBUTE_TYPE_AUTO:
		return strdup("auto");
		default:

		return NULL;
	}
}

EGUEB_DOM_VALUE_ENUM_BOILERPLATE(egueb_smil_attribute_type,
		Egueb_Smil_Attribute_Type);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
const Egueb_Dom_Value_Descriptor * egueb_smil_value_attribute_type_descriptor_get(void)
{
	return &_egueb_smil_attribute_type_descriptor;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

