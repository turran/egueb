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

#ifndef _EGUEB_SMIL_DURATION_H_
#define _EGUEB_SMIL_DURATION_H_

typedef enum _Egueb_Smil_Duration_Type
{
	EGUEB_SMIL_DURATION_TYPE_CLOCK,
	EGUEB_SMIL_DURATION_TYPE_MEDIA,
	EGUEB_SMIL_DURATION_TYPE_INDEFINITE,
} Egueb_Smil_Duration_Type;

typedef struct _Egueb_Smil_Duration
{
	Egueb_Smil_Duration_Type type;
	union {
		Egueb_Smil_Clock clock;
	} data;
} Egueb_Smil_Duration;

extern const Egueb_Smil_Duration EGUEB_SMIL_DURATION_INDEFINITE;

#endif
