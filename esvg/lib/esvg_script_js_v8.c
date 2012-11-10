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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                          The script  interface                             *
 *----------------------------------------------------------------------------*/
static Esvg_Script_Descriptor _descriptor = {

};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_script_js_v8_init(void)
{
	esvg_script_descriptor_register(&descriptor, "application/ecmascript");
}

void esvg_script_js_v8_shutdown(void)
{
	esvg_script_descriptor_unregister(&descriptor, "application/ecmascript");
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

