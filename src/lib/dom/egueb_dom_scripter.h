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
typedef Eina_Bool (*Egueb_Dom_Scripter_Descriptor_Load)(void *prv, Egueb_Dom_String *s, void **obj);
typedef void (*Egueb_Dom_Scripter_Descriptor_Global_Add)(void *prv, const char *name, void *o, Ender_Item *i);
typedef void (*Egueb_Dom_Scripter_Descriptor_Global_Clear)(void *prv);
typedef void (*Egueb_Dom_Scripter_Descriptor_Script_Free)(void *prv, void *obj);
typedef Eina_Bool (*Egueb_Dom_Scripter_Descriptor_Script_Run)(void *prv, void *obj);
typedef Eina_Bool (*Egueb_Dom_Scripter_Descriptor_Script_Run_Listener)(void *prv, void *obj, Egueb_Dom_Event *e);

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
	Egueb_Dom_Scripter_Descriptor_Global_Add global_add;
	Egueb_Dom_Scripter_Descriptor_Global_Clear global_clear;
	Egueb_Dom_Scripter_Descriptor_Script_Free script_free;
	Egueb_Dom_Scripter_Descriptor_Script_Run script_run;
	Egueb_Dom_Scripter_Descriptor_Script_Run_Listener script_run_listener;
} Egueb_Dom_Scripter_Descriptor;

/**
 * @}
 * @defgroup Egueb_Dom_Scripter Scripter
 * @{
 */

typedef struct _Egueb_Dom_Scripter Egueb_Dom_Scripter;

EAPI Egueb_Dom_Scripter * egueb_dom_scripter_new(const Egueb_Dom_Scripter_Descriptor *d);
EAPI void egueb_dom_scripter_free(Egueb_Dom_Scripter *thiz);
EAPI void * egueb_dom_scripter_data_get(Egueb_Dom_Scripter *thiz);
EAPI Eina_Bool egueb_dom_scripter_load(Egueb_Dom_Scripter *thiz, Egueb_Dom_String *s, void **obj);
EAPI void egueb_dom_scripter_global_add(Egueb_Dom_Scripter *thiz, const char *name, void *o, Ender_Item *i);
EAPI void egueb_dom_scripter_global_clear(Egueb_Dom_Scripter *thiz);

EAPI void egueb_dom_scripter_script_free(Egueb_Dom_Scripter *thiz, void *obj);
EAPI Eina_Bool egueb_dom_scripter_script_run(Egueb_Dom_Scripter *thiz, void *obj);
EAPI Eina_Bool egueb_dom_scripter_script_run_listener(Egueb_Dom_Scripter *thiz, void *obj, Egueb_Dom_Event *e);

/**
 * @}
 */

#endif
