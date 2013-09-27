/* Ecss - CSS
 * Copyright (C) 2011 Jorge Luis Zapata
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
# include <config.h>
#endif

#include <Eina.h>

#include "Egueb_Css.h"
#include "ecss_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

static int ecss_init_count = 0;

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

int _ecss_log_dom_global = -1;

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI int ecss_init(void)
{
	if (++ecss_init_count != 1)
		return ecss_init_count;

	if (!eina_init())
	{
		fprintf(stderr, "Ecss: Eina init failed");
		return --ecss_init_count;
	}

	_ecss_log_dom_global = eina_log_domain_register("ecss", ECSS_DEFAULT_LOG_COLOR);
	if (_ecss_log_dom_global < 0)
	{
		EINA_LOG_ERR("Ecss Can not create a general log domain.");
		goto shutdown_eina;
	}

	return ecss_init_count;

shutdown_eina:
	eina_shutdown();
	return --ecss_init_count;
}

EAPI int ecss_shutdown(void)
{
	if (--ecss_init_count != 0)
		return ecss_init_count;

	eina_log_domain_unregister(_ecss_log_dom_global);
	_ecss_log_dom_global = -1;
	eina_shutdown();

	return ecss_init_count;
}
