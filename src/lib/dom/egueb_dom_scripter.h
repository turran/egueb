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
#ifndef _EGUEB_DOM_SCRIPTER_H_
#define _EGUEB_DOM_SCRIPTER_H_

/**
 * @file
 * @listgroup{Egueb_Dom_Scripter_Descriptor_Definition}
 * @listgroup{Egueb_Dom_Scripter_Descriptor}
 * @listgroup{Egueb_Dom_Scripter}
 */

/**
 * @defgroup Egueb_Dom_Scripter_Descriptor_Definition Scripter Descriptor Definition
 * @ingroup Egueb_Dom_Scripter_Descriptor
 * @{
 */

typedef void * (*Egueb_Dom_Scripter_Descriptor_New)(void);
typedef void (*Egueb_Dom_Scripter_Descriptor_Free)(void *prv);
typedef Eina_Bool (*Egueb_Dom_Scripter_Descriptor_Load)(void *prv, Egueb_Dom_String *s);
typedef Eina_Bool (*Egueb_Dom_Scripter_Descriptor_Run)(void *prv);
typedef Eina_Bool (*Egueb_Dom_Scripter_Descriptor_Call)(void *prv, Egueb_Dom_String *s);

/**
 * @}
 * @defgroup Egueb_Dom_Scripter_Descriptor Scripter Descriptor
 * @ingroup Egueb_Dom_Scripter
 * @{
 */

typedef struct _Egueb_Dom_Scripter_Descriptor
{
	Egueb_Dom_Scripter_Descriptor_New new;
	Egueb_Dom_Scripter_Descriptor_Free free;
	Egueb_Dom_Scripter_Descriptor_Load load;
	Egueb_Dom_Scripter_Descriptor_Run run;
	Egueb_Dom_Scripter_Descriptor_Call call;
} Egueb_Dom_Scripter_Descriptor;

/**
 * @}
 * @defgroup Egueb_Dom_Scripter Scripter
 * @{
 */

typedef struct _Egueb_Dom_Scripter Egueb_Dom_Scripter;

EAPI Egueb_Dom_Scripter * egueb_dom_scripter_new(const Egueb_Dom_Scripter_Descriptor *d);
EAPI void * egueb_dom_scripter_data_get(Egueb_Dom_Scripter *thiz);
EAPI Eina_Bool egueb_dom_scripter_load(Egueb_Dom_Scripter *thiz, Egueb_Dom_String *s);
EAPI Eina_Bool egueb_dom_scripter_run(Egueb_Dom_Scripter *thiz);
EAPI Eina_Bool egueb_dom_scripter_call(Egueb_Dom_Scripter *thiz, Egueb_Dom_String *s);

/**
 * @}
 */

#endif
