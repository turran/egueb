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
#ifndef _ECSS_PRIVATE_H
#define _ECSS_PRIVATE_H

extern int _ecss_log_dom_global;

/*
 * Macros that are used everywhere
 *
 * the first four macros are the general macros for the lib
 */
#ifdef ECSS_DEFAULT_LOG_COLOR
# undef ECSS_DEFAULT_LOG_COLOR
#endif
#define ECSS_DEFAULT_LOG_COLOR EINA_COLOR_CYAN

#ifdef ERR
# undef ERR
#endif /* ifdef ERR */
#define ERR(...)  EINA_LOG_DOM_ERR(_ecss_log_dom_global, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif /* ifdef DBG */
#define DBG(...)  EINA_LOG_DOM_DBG(_ecss_log_dom_global, __VA_ARGS__)

#ifdef INF
# undef INF
#endif /* ifdef INF */
#define INF(...)  EINA_LOG_DOM_INFO(_ecss_log_dom_global, __VA_ARGS__)

#ifdef WRN
# undef WRN
#endif /* ifdef WRN */
#define WRN(...)  EINA_LOG_DOM_WARN(_ecss_log_dom_global, __VA_ARGS__)

#ifdef CRIT
# undef CRIT
#endif /* ifdef CRIT */
#define CRIT(...) EINA_LOG_DOM_CRIT(_ecss_log_dom_global, __VA_ARGS__)


typedef Eina_Bool (*Ecss_Filter_Test)(void *data, Ecss_Context *c, Ecss_Element *e);
typedef void (*Ecss_Filter_Free)(void *data);

typedef struct _Ecss_Filter_Descriptor
{
	Ecss_Filter_Test test;
	Ecss_Filter_Free free;
} Ecss_Filter_Descriptor;

Ecss_Filter * ecss_filter_new(Ecss_Filter_Descriptor *fd, void *data);
Eina_Bool ecss_filter_test(Ecss_Filter *f, Ecss_Context *c, Ecss_Element *e);
void ecss_filter_delete(Ecss_Filter *f);

typedef struct _Ecss_Parser
{
	Ecss_Style *style;
} Ecss_Parser;

typedef struct _Ecss_Selector_Combinator
{
	Ecss_Selector *s;
	Ecss_Combinator c;
} Ecss_Selector_Combinator;

struct _Ecss_Selector
{
	EINA_INLIST;
	char *subject;
	Ecss_Combinator c;
	Eina_List *filters;
};

typedef struct _Ecss_Declaration
{
	char *property;
	char *value;
} Ecss_Declaration;

struct _Ecss_Rule
{
	Ecss_Selector *selector;
	Eina_List *declarations;
};

struct _Ecss_Style
{
	Eina_List *rules;
};

#endif
