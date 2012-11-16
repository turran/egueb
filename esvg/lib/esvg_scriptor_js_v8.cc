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
/*
 * We need to implement functions on ender, that way we can introspect
 * correctly an object, based on its atttributes (properties for ender)
 * and functions. I suppose that whenever a function return an ender
 * element, we need to wrap that function so we can assign the prototype
 * of the returned element based on some lookup of its ender descriptor
 *
 * For getters/setters it is very simple, we just call the
 * ender_element_property_set. But with a value? or without?
 *
 * We need to add the ender element passed on the context new
 * as the global object
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Scriptor_Js_V8
{
	Ender_Element *svg;
	Persistent<Context> context;
} Esvg_Scriptor_Js_V8;

static void _v8_element_property_to_js(Ender_Property *prop,
		void *data)
{

}

static void _v8_element_descriptor_to_js(Esvg_Scriptor_Js_V8 *thiz,
		Ender_Descriptor *descriptor)
{
	/* start adding the properties for such descriptor */
	ender_descriptor_property_list(descriptor, _v8_element_property_to_js,
			NULL);
	/* TODO add the functions */
	/* get the parent descriptor and do the same */
	descriptor = ender_descriptor_parent(descriptor);
	if (!descriptor) return;

	_v8_element_descriptor_to_js(thiz, descriptor);
}

static void _v8_element_to_js(Esvg_Scriptor_Js_V8 *thiz, Ender_Element *e)
{
	Ender_Descriptor *descriptor;

	descriptor = ender_element_descriptor_get(e);
	/* TODO in case the descriptor is already registered dont do nothing */
	_v8_element_descriptor_to_js(thiz, descriptor);
}

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

