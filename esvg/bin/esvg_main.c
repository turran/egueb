/* ESVG - SVG
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include <Esvg.h>

int main(int argc, char *argv[])
{
	Enesim_Renderer *r;

	if (argc < 2)
	{
		printf("Usage : %s file.svg\n", argv[0]);
		return -1;
	}

	if (!esvg_init())
		return -1;

	r = esvg_parser_load(argv[1]);
	if (!r)
	{
		printf("Fail to parse %s\n", argv[1]);
	}

	esvg_shutdown();

	return 0;
}
