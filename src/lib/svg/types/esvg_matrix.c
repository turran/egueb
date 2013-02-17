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

#include "esvg_private_main.h"

#include "esvg_types.h"
#include "esvg_private_types.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_matrix_log

static int _esvg_matrix_log = -1;

static Ender_Property *ESVG_MATRIX_A;
static Ender_Property *ESVG_MATRIX_B;
static Ender_Property *ESVG_MATRIX_C;
static Ender_Property *ESVG_MATRIX_D;
static Ender_Property *ESVG_MATRIX_E;
static Ender_Property *ESVG_MATRIX_F;

static Ender_Property *ESVG_MATRIX_ANIMATED_BASE;
static Ender_Property *ESVG_MATRIX_ANIMATED_ANIM;

static void _esvg_matrix_a_get(Esvg_Matrix *thiz, double *v)
{
	*v = thiz->xx;
}

static void _esvg_matrix_b_get(Esvg_Matrix *thiz, double *v)
{
	*v = thiz->yx;
}

static void _esvg_matrix_c_get(Esvg_Matrix *thiz, double *v)
{
	*v = thiz->xy;
}

static void _esvg_matrix_d_get(Esvg_Matrix *thiz, double *v)
{
	*v = thiz->yy;
}

static void _esvg_matrix_e_get(Esvg_Matrix *thiz, double *v)
{
	*v = thiz->xz;
}

static void _esvg_matrix_f_get(Esvg_Matrix *thiz, double *v)
{
	*v = thiz->yz;
}

#define _esvg_matrix_new NULL
#define _esvg_matrix_delete NULL
#define _esvg_matrix_a_set NULL
#define _esvg_matrix_a_is_set NULL
#define _esvg_matrix_b_set NULL
#define _esvg_matrix_b_is_set NULL
#define _esvg_matrix_c_set NULL
#define _esvg_matrix_c_is_set NULL
#define _esvg_matrix_d_set NULL
#define _esvg_matrix_d_is_set NULL
#define _esvg_matrix_e_set NULL
#define _esvg_matrix_e_is_set NULL
#define _esvg_matrix_f_set NULL
#define _esvg_matrix_f_is_set NULL
#include "esvg_generated_matrix.c"

#define _esvg_matrix_animated_new NULL
#define _esvg_matrix_animated_delete NULL
#define _esvg_matrix_animated_base_set NULL
#define _esvg_matrix_animated_base_get NULL
#define _esvg_matrix_animated_base_is_set NULL
#define _esvg_matrix_animated_anim_set NULL
#define _esvg_matrix_animated_anim_get NULL
#define _esvg_matrix_animated_anim_is_set NULL
#include "esvg_generated_matrix_animated.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_matrix_init(void)
{
	_esvg_matrix_log = eina_log_domain_register("esvg_matrix", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_matrix_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_matrix_init();
	_esvg_matrix_animated_init();
}

void esvg_matrix_shutdown(void)
{
	_esvg_matrix_shutdown();
	_esvg_matrix_animated_shutdown();
}

