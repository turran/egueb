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
#include <jsapi.h>
#include <stdlib.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#if 0
int main(int argc, char **argv)
{
	static JSClass global_class = {
		"global", JSCLASS_GLOBAL_FLAGS,
		JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
		JSCLASS_NO_OPTIONAL_MEMBERS
	};

	JSRuntime *rt;
	JSContext *cx;
	JSObject  *global;

	/* Create an instance of the engine */

	rt = JS_NewRuntime(1024*1024);

	if (!rt) {
		exit(EXIT_FAILURE);
	}

	/* Create an execution context */

	cx = JS_NewContext(rt, 8192);

	if (!cx) {
		exit(EXIT_FAILURE);
	}

	/* Create a global object and a set of standard objects */

	global = JS_NewObject(cx, &global_class, NULL, NULL);

	if (!global) {
		exit(EXIT_FAILURE);
	}

	if (JS_InitStandardClasses(cx, global) != JS_TRUE) {
		exit(EXIT_FAILURE);
	}

	/* Do something */

	...

	/* Cleanup */

	JS_DestroyContext(cx);
	JS_DestroyRuntime(rt);
	JS_ShutDown();

	exit(EXIT_SUCCESS);
}
#endif
/*----------------------------------------------------------------------------*
 *                          The script  interface                             *
 *----------------------------------------------------------------------------*/
static Esvg_Script_Descriptor _descriptor = {

};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_script_js_sm_init(void)
{
	esvg_script_descriptor_register(&descriptor, "application/ecmascript");
}

void esvg_script_js_sm_shutdown(void)
{
	esvg_script_descriptor_unregister(&descriptor, "application/ecmascript");
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/


