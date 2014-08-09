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

#ifndef _EGUEB_SMIL_TIMELINE_H_
#define _EGUEB_SMIL_TIMELINE_H_

/**
 * @defgroup Egueb_Smil_Timeline Timeline
 * @ingroup Egueb_Smil_Group
 * @{
 */

typedef struct _Egueb_Smil_Timeline Egueb_Smil_Timeline;

/* for forward declarations */
#include "egueb_smil_signal.h"

EAPI Egueb_Smil_Timeline * egueb_smil_timeline_new(void);
EAPI Egueb_Smil_Timeline * egueb_smil_timeline_ref(Egueb_Smil_Timeline *thiz);
EAPI void egueb_smil_timeline_unref(Egueb_Smil_Timeline *thiz);
EAPI void egueb_smil_timeline_signal_add(Egueb_Smil_Timeline *thiz, Egueb_Smil_Signal *s);
EAPI void egueb_smil_timeline_signal_remove(Egueb_Smil_Timeline *thiz, Egueb_Smil_Signal *s);

/**
 * @}
 */

#endif
