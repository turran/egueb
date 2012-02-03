/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#include "Esvg.h"
#include "esvg_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Esvg_Length ESVG_LENGTH_0 = { 0.0, ESVG_UNIT_LENGTH_PX };
Esvg_Length ESVG_LENGTH_1 = { 1.0, ESVG_UNIT_LENGTH_PX };
Esvg_Length ESVG_LENGTH_100_PERCENT = { 100.0, ESVG_UNIT_LENGTH_PERCENT };
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
