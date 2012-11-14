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

#include <v8.h>
using namespace v8;
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Scriptor_Js_V8
{
	Persistent<Context> context;
} Esvg_Scriptor_Js_V8;

v8::Handle<v8::Value> Alert(const v8::Arguments& args)
{
	v8::String::Utf8Value str(args[0]); // Convert first argument to V8 String
	//const char* cstr = ToCString(str); // Convert V8 String to C string

	/* call the svg with the string */
	return v8::Undefined();
}

#if 0
  // Create a stack-allocated handle scope.
  HandleScope handle_scope;
  // Create a new context.
  Handle<Context> context = Context::New();
  // Enter the created context for compiling and
  // running the hello world script.
  Context::Scope context_scope(context);
  // Create a string containing the JavaScript source code.
  Handle<String> source = String::New("'Hello' + ', World!'");
  // Compile the source code.
  Handle<Script> script = Script::Compile(source);
  // Run the script to get the result.
  Handle<Value> result = script->Run();
  // Convert the result to an ASCII string and print it.
  String::AsciiValue ascii(result);
  printf("%s\n", *ascii);
  return 0;
#endif
/*----------------------------------------------------------------------------*
 *                          The script  interface                             *
 *----------------------------------------------------------------------------*/
static void * _v8_context_new(void)
{
	Esvg_Scriptor_Js_V8 *thiz;

	thiz = (Esvg_Scriptor_Js_V8 *)calloc(1, sizeof(Esvg_Scriptor_Js_V8));
	thiz->context = Context::New();
	/* TODO register all the types that we own?
	 * i.e the js idl?
	 */
	/* add default implementation of the alert message
	global->Set(v8::String::New("alert"), v8::FunctionTemplate::New(Alert));
	*/

	return thiz;
}

static void _v8_context_free(void *ctx)
{
	Esvg_Scriptor_Js_V8 *thiz = (Esvg_Scriptor_Js_V8 *)ctx;

	thiz->context.Dispose();
	free(thiz);
}

static void _v8_run(void *ctx, const char *script)
{

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

