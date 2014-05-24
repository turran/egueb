/* Egueb_Dom - DOM
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

#ifndef _EGUEB_DOM_PRIVATE_H
#define _EGUEB_DOM_PRIVATE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Eina.h>
#include <Enesim.h>
#include <Etch.h>

#include <Egueb_Base.h>

#include "egueb_dom_build.h"

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(egueb_dom_log_dom_global, __VA_ARGS__)

#ifdef WARN
# undef WARN
#endif
#define WARN(...) EINA_LOG_DOM_WARN(egueb_dom_log_dom_global, __VA_ARGS__)

#ifdef INFO
# undef INFO
#endif
#define INFO(...) EINA_LOG_DOM_INFO(egueb_dom_log_dom_global, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(egueb_dom_log_dom_global, __VA_ARGS__)

#ifdef CRIT
# undef CRIT
#endif
#define CRIT(...) EINA_LOG_DOM_CRIT(egueb_dom_log_dom_global, __VA_ARGS__)

/* Element based logs */
#ifdef ERR_ELEMENT
# undef ERR_ELEMENT
#endif
#define ERR_ELEMENT(n, ...) EGUEB_DOM_ELEMENT_LOG_DOM_ERR(n, egueb_dom_log_dom_global, __VA_ARGS__)

#ifdef WARN_ELEMENT
# undef WARN_ELEMENT
#endif
#define WARN_ELEMENT(n, ...) EGUEB_DOM_ELEMENT_LOG_DOM_WARN(n, egueb_dom_log_dom_global, __VA_ARGS__)

#ifdef INFO_ELEMENT
# undef INFO_ELEMENT
#endif
#define INFO_ELEMENT(n, ...) EGUEB_DOM_ELEMENT_LOG_DOM_INFO(n, egueb_dom_log_dom_global, __VA_ARGS__)

#ifdef DBG_ELEMENT
# undef DBG_ELEMENT
#endif
#define DBG_ELEMENT(n, ...) EGUEB_DOM_ELEMENT_LOG_DOM_DBG(n, egueb_dom_log_dom_global, __VA_ARGS__)

#ifdef CRIT_ELEMENT
# undef CRIT_ELEMENT
#endif
#define CRIT_ELEMENT(n, ...) EGUEB_DOM_ELEMENT_LOG_DOM_CRIT(n, egueb_dom_log_dom_global, __VA_ARGS__)

extern int egueb_dom_log_dom_global;

/* parser */
/* tag */

#endif
