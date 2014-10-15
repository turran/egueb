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

#ifndef _EGUEB_SMIL_CLOCK_H_
#define _EGUEB_SMIL_CLOCK_H_

#define EGUEB_SMIL_CLOCK_SECONDS (1000000000LL)
#define EGUEB_SMIL_CLOCK_MSECONDS (1000000LL)
#define EGUEB_SMIL_CLOCK_AS_SECONDS(t) ((t) / EGUEB_SMIL_CLOCK_SECONDS)

#define EGUEB_SMIL_CLOCK_FORMAT "u:%02u:%02u.%09u"
#define EGUEB_SMIL_CLOCK_ARGS(t) \
	(uint32_t) (((Egueb_Smil_Clock)(t)) / (EGUEB_SMIL_CLOCK_SECONDS * 60 * 60)), \
	(uint32_t) ((((Egueb_Smil_Clock)(t)) / (EGUEB_SMIL_CLOCK_SECONDS * 60)) % 60), \
	(uint32_t) ((((Egueb_Smil_Clock)(t)) / EGUEB_SMIL_CLOCK_SECONDS) % 60), \
	(uint32_t) (((Egueb_Smil_Clock)(t)) % EGUEB_SMIL_CLOCK_SECONDS)

typedef int64_t Egueb_Smil_Clock; /**< Clock abstraction in nano seconds */

#endif
