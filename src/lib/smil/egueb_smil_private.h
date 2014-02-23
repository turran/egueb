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
#include <Egueb_Dom.h>

#include "egueb_smil_build.h"

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(egueb_smil_log_global, __VA_ARGS__)

#ifdef WARN
# undef WARN
#endif
#define WARN(...) EINA_LOG_DOM_WARN(egueb_smil_log_global, __VA_ARGS__)

#ifdef INFO
# undef INFO
#endif
#define INFO(...) EINA_LOG_DOM_INFO(egueb_smil_log_global, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(egueb_smil_log_global, __VA_ARGS__)

extern int egueb_smil_log_global;

/* parser */
/* tag */

#endif

