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
#include "egueb_smil_clock.h"
#include "egueb_smil_timing.h"

#include "egueb_smil_clock_private.h"
#include "egueb_smil_timing_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static inline Eina_Bool _is_a_z(char v)
{
	if (v >= 'a' && v <= 'z')
		return EINA_TRUE;
	return EINA_FALSE;
}

static inline Eina_Bool _is_A_Z(char v)
{
	if (v >= 'A' && v <= 'Z')
		return EINA_TRUE;
	return EINA_FALSE;
}

static inline Eina_Bool _is_Aa_Zz(char v)
{
	if (_is_A_Z(v) || _is_a_z(v))
		return EINA_TRUE;
	return EINA_FALSE;
}

static inline Eina_Bool _is_0_9(char v)
{
	if (v >= '0' && v <= '9')
		return EINA_TRUE;
	return EINA_FALSE;
}

static Eina_Bool _is_name_first(char v)
{
	/* FIXME here we should use the correct one from the xml doc:
	 * http://www.w3.org/TR/REC-xml/#NT-NameStartChar
	 */
	if (_is_Aa_Zz(v) || v == ':' || v == '_')
		return EINA_TRUE;
	return EINA_FALSE;
}

static Eina_Bool _is_name(char v)
{
	if (_is_name_first(v) || v == '-' || v == '.' || _is_0_9(v))
		return EINA_TRUE;
	return EINA_FALSE;
}

static Eina_Bool _animation_clock_parse(const char *v, int64_t *clock)
{
	Eina_Bool n = EINA_FALSE;
	if (*v == '+')
		v++;
	else if (*v == '-')
	{
		v++;
		n = EINA_TRUE;
	}
	if (!egueb_smil_clock_string_from(clock, v))
		return EINA_FALSE;

	if (n)
		*clock = -(*clock);
	return EINA_TRUE;
}

static Eina_Bool _animation_name_parse(const char *v, const char **start, int *len)
{
	const char *s;

	/* check the first letter */
	EGUEB_DOM_SPACE_SKIP(v);

	s = v;
	if (!_is_name_first(*v))
		return EINA_FALSE;
	v++;

	/* then for each, iterate until we find the last valid char */
	while (_is_name(*v) && *v != '-' && *v != '.')
	{
		v++;
	}
	*start = s;
	*len = v - *start;
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_smil_timing_string_from(Egueb_Smil_Timing *thiz,
		const char *s)
{
	Eina_Bool ret;
	const char *start;
	const char *end;
	int len;

	/* just in case */
	thiz->id = NULL;
	thiz->event = NULL;

	ret = _animation_name_parse(s, &start, &len);
	end = start + len;
	/* no name, just numbers ... clock */
	if (!ret)
	{
		/* clock */
		if (!_animation_clock_parse(s, &thiz->offset))
			return EINA_FALSE;
	}
	else
	{
		/* simplest cases */
		if (!strcmp(s, "indefinite"))
		{
			/* TODO */
			ERR("Indefinite not supported yet");
		}
		else if (!strncmp(s, "accessKey", 9))
		{
			/* TODO */
			ERR("AccesKey not supported yet");
		}
		else if (!strncmp(s, "wallclock", 9))
		{
			/* TODO */
			ERR("Wallclock not supported yet");
		}
		else
		{
			/* id */
			if (*end == '.')
			{
				thiz->id = egueb_dom_string_new_with_length(start, len);
				/* advance after the point */
				ret = _animation_name_parse(end + 1, &start, &len);
				end = start + len;
				if (!ret) goto err;
			}
			/* repeat */
			if (!strncmp(s, "repeat", len))
			{
				long r;
				if (*end == '(')
					start++;
				if (!egueb_dom_long_get(start, &end, &r))
					goto err;
				if (*end != ')')
					goto err;
				thiz->repeat = r;
			}
			/* event name */
			else
			{
				thiz->event = egueb_dom_string_new_with_length(start, len);
			}
			/* offset */
			if (*end)
				_animation_clock_parse(end + 1, &thiz->offset);
		}
	}
	return EINA_TRUE;
err:
	egueb_smil_timing_reset(thiz);
	return EINA_FALSE;
}

char * egueb_smil_timing_string_to(Egueb_Smil_Timing *thiz)
{
	ERR("Not implemented");
	return NULL;
}

void egueb_smil_timing_copy(const Egueb_Smil_Timing *thiz, Egueb_Smil_Timing *copy, Eina_Bool full)
{
	egueb_smil_timing_reset(copy);
	if (full)
	{
		copy->id = egueb_dom_string_dup(thiz->id);
		copy->event = egueb_dom_string_dup(thiz->event);
	}
	else
	{
		copy->id = egueb_dom_string_ref(thiz->id);
		copy->event = egueb_dom_string_ref(thiz->event);
	}
	copy->type = thiz->type;
	copy->repeat = thiz->repeat;
	copy->key = thiz->key;
	copy->offset = thiz->offset;
}

void egueb_smil_timing_interpolate(Egueb_Smil_Timing *v,
		Egueb_Smil_Timing *a, Egueb_Smil_Timing *b, double m,
		Egueb_Smil_Timing *add, Egueb_Smil_Timing *acc, int mul)
{
	ERR("Not implemented");
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_smil_timing_reset(Egueb_Smil_Timing *thiz)
{
	egueb_dom_string_unref(thiz->id);
	egueb_dom_string_unref(thiz->event);
}
