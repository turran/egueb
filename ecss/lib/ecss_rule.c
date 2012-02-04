/* Ecss - CSS
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

#include "Ecss.h"
#include "ecss_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void ecss_rule_declaration_insert(Ecss_Rule *thiz, Ecss_Declaration *d)
{
	printf("New declaration of %s %s\n", d->property, d->value);
	thiz->declarations = eina_list_append(thiz->declarations, d);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ecss_Rule * ecss_rule_new(Ecss_Selector *s)
{
	Ecss_Rule *thiz;

	if (!s) return NULL;

	thiz = calloc(1, sizeof(Ecss_Rule));
	thiz->selector = s;

	return thiz;
}

EAPI void ecss_rule_declaration_add(Ecss_Rule *thiz, const char *property,
		const char *value)
{
	Ecss_Declaration *d;

	if (!property || !value) return;

	d = malloc(sizeof(Ecss_Declaration));
	d->property = strdup(property);
	d->value = strdup(value);

	ecss_rule_declaration_insert(thiz, d);
}
