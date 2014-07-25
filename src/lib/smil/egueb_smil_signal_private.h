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

#ifndef _EGUEB_SMIL_SIGNAL_PRIVATE_H_
#define _EGUEB_SMIL_SIGNAL_PRIVATE_H_

struct _Egueb_Smil_Signal
{
	Enesim_Object_Instance parent;
	Egueb_Smil_Timeline *timeline;
	Eina_Bool enabled;/** easy way to disable/enable an animation */
	Etch_Time offset; /*  the real offset */
	int ref;

	Eina_Bool started;
	Eina_Bool stopped;
	Egueb_Smil_Signal_State_Callback start_cb;
	Egueb_Smil_Signal_State_Callback stop_cb;
	void *data;
};

typedef void (*Egueb_Smil_Signal_Process)(Egueb_Smil_Signal *thiz,
	Egueb_Smil_Clock curr, Egueb_Smil_Clock tpf);

typedef struct _Egueb_Smil_Signal_Class
{
	Enesim_Object_Class parent;
	Egueb_Smil_Signal_Process process;
} Egueb_Smil_Signal_Class;

#define EGUEB_SMIL_SIGNAL_DESCRIPTOR egueb_smil_signal_descriptor_get()
Enesim_Object_Descriptor * egueb_smil_signal_descriptor_get(void);

#define EGUEB_SMIL_SIGNAL_CLASS_GET(o) EGUEB_SMIL_SIGNAL_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SMIL_SIGNAL_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,			\
		Egueb_Smil_Signal_Class, EGUEB_SMIL_SIGNAL_DESCRIPTOR)
#define EGUEB_SMIL_SIGNAL(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Smil_Signal, EGUEB_SMIL_SIGNAL_DESCRIPTOR)

void egueb_smil_signal_process(Egueb_Smil_Signal *thiz, Egueb_Smil_Clock curr, unsigned int tpf);
void egueb_smil_signal_start(Egueb_Smil_Signal *thiz);
void egueb_smil_signal_stop(Egueb_Smil_Signal *thiz);

#endif
