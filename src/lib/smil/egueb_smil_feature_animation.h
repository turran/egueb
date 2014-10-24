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
#ifndef _EGUEB_SMIL_FEATURE_ANIMATION_H_
#define _EGUEB_SMIL_FEATURE_ANIMATION_H_

EAPI extern Egueb_Dom_String *EGUEB_SMIL_FEATURE_ANIMATION_NAME;

typedef void (*Egueb_Smil_Feature_Animation_On_Tick)(Egueb_Dom_Node *n);

#define EGUEB_SMIL_FEATURE_ANIMATION_DESCRIPTOR_VERSION 0
typedef struct _Egueb_Smil_Feature_Animation_Descriptor
{
	int version;
	Egueb_Smil_Feature_Animation_On_Tick on_tick; 
} Egueb_Smil_Feature_Animation_Descriptor;

EAPI Eina_Bool egueb_smil_feature_animation_has_animations(Egueb_Dom_Feature *f);
EAPI Eina_Bool egueb_smil_feature_animation_fps_set(Egueb_Dom_Feature *f, int fps);
EAPI Eina_Bool egueb_smil_feature_animation_fps_get(Egueb_Dom_Feature *f, int *fps);
EAPI Eina_Bool egueb_smil_feature_animation_duration_get(Egueb_Dom_Feature *f,
		Egueb_Smil_Clock *clock);
EAPI Eina_Bool egueb_smil_feature_animation_tick(Egueb_Dom_Feature *f);
EAPI void egueb_smil_feature_animation_pause(Egueb_Dom_Feature *f);
EAPI void egueb_smil_feature_animation_unpause(Egueb_Dom_Feature *f);
EAPI Eina_Bool egueb_smil_feature_animation_paused(Egueb_Dom_Feature *f);

EAPI Eina_Bool egueb_smil_feature_animation_add(Egueb_Dom_Node *n,
		const Egueb_Smil_Feature_Animation_Descriptor *descriptor);

#endif
