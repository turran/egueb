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

#ifndef _EGUEB_SMIL_TIMELINE_PRIVATE_H_
#define _EGUEB_SMIL_TIMELINE_PRIVATE_H_

typedef struct _Egueb_Smil_Timeline Egueb_Smil_Timeline;

/* for forward declarations */
#include "egueb_smil_signal_private.h"

void egueb_smil_timeline_tick(Egueb_Smil_Timeline *thiz);
unsigned int egueb_smil_timeline_tpf_get(Egueb_Smil_Timeline *thiz);
void egueb_smil_timeline_fps_set(Egueb_Smil_Timeline *thiz, unsigned int fps);
unsigned int egueb_smil_timeline_fps_get(Egueb_Smil_Timeline *thiz);
Egueb_Smil_Clock egueb_smil_timeline_current_clock_get(Egueb_Smil_Timeline *thiz);

Egueb_Smil_Timeline * egueb_smil_timeline_new(void);
Egueb_Smil_Timeline * egueb_smil_timeline_ref(Egueb_Smil_Timeline *thiz);
void egueb_smil_timeline_unref(Egueb_Smil_Timeline *thiz);
void egueb_smil_timeline_signal_add(Egueb_Smil_Timeline *thiz, Egueb_Smil_Signal *s);
void egueb_smil_timeline_signal_remove(Egueb_Smil_Timeline *thiz, Egueb_Smil_Signal *s);

#endif
