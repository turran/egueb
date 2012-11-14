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
#include "esvg_private_main.h"
#include "esvg_private_scriptor.h"
#include "esvg_private_svg.h"

#include <v8.h>
using namespace v8;
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Scriptor_Js_V8
{
	Ender_Element *svg;
	Persistent<Context> context;
} Esvg_Scriptor_Js_V8;

v8::Handle<v8::Value> Alert(const v8::Arguments& args)
{
	Esvg_Scriptor_Js_V8 *thiz;

	String::Utf8Value str(args[0]); // Convert first argument to V8 String
	Local<External> wrap = Local<External>::Cast(args.Data());
	thiz = static_cast<Esvg_Scriptor_Js_V8*>(wrap->Value());

	/* call the svg with the string */
	esvg_svg_script_alert(thiz->svg, *str);
	return Undefined();
}
/*----------------------------------------------------------------------------*
 *                          The script  interface                             *
 *----------------------------------------------------------------------------*/
static void * _v8_context_new(Ender_Element *e)
{
	Esvg_Scriptor_Js_V8 *thiz;
	HandleScope handle_scope;
	Persistent<Context> context;

	thiz = (Esvg_Scriptor_Js_V8 *)calloc(1, sizeof(Esvg_Scriptor_Js_V8));
	/* create the global object */
	Handle<ObjectTemplate> global = ObjectTemplate::New();
	/* add default implementation of the alert message */
	global->Set(String::New("alert"), FunctionTemplate::New(Alert, External::New(thiz)));

	context = Context::New(NULL, global);
	/* TODO register all the types that we own?
	 * i.e the js idl?
	 */
	thiz->context = context;
	thiz->svg = e;

	return thiz;
}

static void _v8_context_free(void *ctx)
{
	Esvg_Scriptor_Js_V8 *thiz = (Esvg_Scriptor_Js_V8 *)ctx;

	thiz->context.Dispose();
	free(thiz);
}

static void _v8_run(void *ctx, const char *s)
{
	Esvg_Scriptor_Js_V8 *thiz = (Esvg_Scriptor_Js_V8 *)ctx;
	HandleScope handle_scope;

	thiz->context->Enter();
	/* create the source string */
	Handle<String> source = String::New(s);
	/* compile it */
	Handle<Script> script = Script::Compile(source);
	/* run it */
 	script->Run();
}

static Esvg_Scriptor_Descriptor _descriptor = {
	/* .context_new 	= */ _v8_context_new,
	/* .context_free	= */ _v8_context_free,
	/* .run 		= */ _v8_run,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_scriptor_js_v8_init(void)
{
	esvg_scriptor_register(&_descriptor, "application/ecmascript");
}

void esvg_scriptor_js_v8_shutdown(void)
{
	esvg_scriptor_unregister(&_descriptor, "application/ecmascript");
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

