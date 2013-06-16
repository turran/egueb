/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include "egueb_svg_main_private.h"

#include "egueb_svg_types.h"
#include "egueb_svg_types_private.h"

#include "egueb_svg_path_private.h"
#define ESVG_LOG_DEFAULT _egueb_svg_path_log

static int _egueb_svg_path_log = -1;

/*----------------------------------------------------------------------------*
 *                            Ender interface Path                            *
 *----------------------------------------------------------------------------*/
#define _egueb_svg_path_seg_list_new egueb_svg_path_seg_list_new
#define _egueb_svg_path_seg_list_delete egueb_svg_path_seg_list_unref
#include "egueb_svg_generated_path_seg_list.c"

