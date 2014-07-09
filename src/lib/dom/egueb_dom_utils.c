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
#include "egueb_dom_private.h"
#include "egueb_dom_utils.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_dom_long_get(const char *iter, const char **tmp, long *l)
{
	char *endptr;
	long val;

	val = strtol(iter, &endptr, 10);
	if ((errno != ERANGE) &&
	    !((val == 0) && (iter == endptr)))
	{
		*tmp = endptr;
		*l = val;
		return EINA_TRUE;
	}

	return EINA_FALSE;
}

EAPI Eina_Bool egueb_dom_double_get(const char *nptr, char **endptr, double *r)
{
	const char *str;
	unsigned long long mantisse = 0;
	unsigned long long dec = 1;
	unsigned long long val = 1;
	double res;
	int has_sign = 0;
	int has_exp_sign = 0;
	int dotted = 0;
	int has_exponent = 0;

	str = nptr;

	if (!str || !*str)
	{
		if (endptr) *endptr = (char *)str;
		return EINA_FALSE;
	}

	if (*str == '-')
	{
		has_sign = 1;
		str++;
	}
	else if (*str == '+')
		str++;

	while (*str)
	{
		if ((*str >= '0') && (*str <= '9'))
		{
			mantisse *= 10;
			mantisse += (*str - '0');
			if (dotted) dec *= 10;
		}
		else if (*str == '.')
		{
			if (dotted)
			{
				if (endptr) *endptr = (char *)str;
				return EINA_FALSE;
			}
			dotted = 1;
		}
		else if ((*str == 'e') || (*str == 'E'))
		{
			str++;
			has_exponent = 1;
			break;
		}
		else
			break;

		str++;
	}

	if (*str && has_exponent)
	{
		int exponent = 0;
		int i;

		has_exponent = 0;
		if (*str == '+') str++;
		else if (*str == '-')
		{
			has_exp_sign = 1;
			str++;
		}
		while (*str)
		{
			if ((*str >= '0') && (*str <= '9'))
			{
				has_exponent = 1;
				exponent *= 10;
				exponent += (*str - '0');
			}
			else
				break;
			str++;
		}

		if (has_exponent)
		{
			for (i = 0; i < exponent; i++)
				val *= 10;
		}
	}

	if (endptr) *endptr = (char *)str;

	if (has_sign)
		res = -(double)mantisse / (double)dec;
	else
		res = (double)mantisse / (double)dec;

	if (val != 1)
	{
		if (has_exp_sign)
			res /= val;
		else
			res *= val;
	}
	*r = res;
	return EINA_TRUE;
}

EAPI Eina_Bool egueb_dom_list_get(const char *attr, char sep, Egueb_Dom_List_Cb cb, void *data)
{
	char *found;

	if (!attr) return EINA_FALSE;
	if (!cb) return EINA_FALSE;

	EGUEB_DOM_SPACE_SKIP(attr);
	while ((found = strchr(attr, sep)))
	{
		*found = '\0';
		EGUEB_DOM_SPACE_SKIP(attr);
		cb(attr, data);
		*found = sep;
		attr = found + 1;
		EGUEB_DOM_SPACE_SKIP(attr);
	}
	if (attr)
		cb(attr, data);

	return EINA_TRUE;
}

/* parse a string of type funcname(a, b, .... numelements)
 * in, out numelements
 * in attr_val
 * out endptr
 * out elements
 */
EAPI Eina_Bool egueb_dom_function_get(const char *attr_val, const char **endptr,
		const char *funcname, int *numelements, double *elements)
{
	int nvalues = 0;
	const char *tmp = attr_val;
	char *end;
	size_t sz;

	sz = strlen(funcname);
	EGUEB_DOM_SPACE_SKIP(tmp);
	if (strncmp(tmp, funcname, sz) != 0)
		return EINA_FALSE;
	tmp += sz;
	EGUEB_DOM_SPACE_SKIP(tmp);
	if (tmp[0] != '(')
		return EINA_FALSE;
	tmp++;
	while (*tmp)
	{
		double val;

		EGUEB_DOM_SPACE_SKIP(tmp);
		if (tmp[0] == ')')
			goto end;
		if (!egueb_dom_double_get(tmp, &end, &val))
			return EINA_FALSE;
		tmp = end;
		elements[nvalues] = val;
		nvalues++;
		/* if we passed the limit, break */
		if (nvalues >= *numelements)
			break;
		/* skip the comma and the blanks */
		EGUEB_DOM_SPACE_COMMA_SKIP(tmp);
	}

	if (tmp[0] != ')')
		return EINA_FALSE;
end:
	tmp++;
	*numelements = nvalues;
	*endptr = tmp;

	return EINA_TRUE;
}


