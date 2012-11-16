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
#include <map>

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

/* FIXME or we either use the stl or an eina hash, but looks like
 * we cannot access the pointer of the persistent handle to pass
 * it to eina_hash?
 */
static std::map<const char *, Persistent<FunctionTemplate> > _prototypes;

#if 0
static Handle<Value> GetPointX(Local<String> property, const AccessorInfo &info)
{
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int value = static_cast<Point*>(ptr)->x_;
	return Integer::New(value);
}

static void SetPointX(Local<String> property, Local<Value> value,
		const AccessorInfo& info)
{
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Point*>(ptr)->x_ = value->Int32Value();
}
#endif

static void _v8_element_property_to_js(Ender_Property *prop,
		void *data)
{
	Ender_Container *c;
	const char *name = (const char *)data;

	/* FIXME we need to figure out how to pass the
	 * handle as a void *
	 */
	Persistent<FunctionTemplate> tmpl = _prototypes[name];
	/* get the container of the property */
	c = ender_property_container_get(prop);
	/* for simple containers it is simple, for
	 * compound containers, we need to use the specification
	 * as found on the svg spec
	 */
	printf("property of name = %s\n", ender_property_name_get(prop));
	/* add an accessor to the template */
}

static Persistent<FunctionTemplate> _v8_element_descriptor_to_js(
		Esvg_Scriptor_Js_V8 *thiz,
		Ender_Descriptor *descriptor)
{
	const char *name;
	const void *ot;

	/* find the descriptor name in the hash of templates */
	name = ender_descriptor_name_get(descriptor);
	Persistent<FunctionTemplate> tmpl = _prototypes[name];

	if (tmpl.IsEmpty())
	{
		Ender_Namespace *ns;
		const char *ns_name = "SVG";
		char real_name[PATH_MAX];

#if 0
		ns = ender_descriptor_namespace_get(descriptor);
		ns_name = ender_namespace_name_get(ns);
#endif
		strcpy(real_name, ns_name);
		strcat(real_name, name);

		/* create a new prototype and store it */
		HandleScope handle_scope;
		Handle<FunctionTemplate> l_tmpl = FunctionTemplate::New();
		l_tmpl->SetClassName(String::New(real_name));
		Handle<ObjectTemplate> o_tmpl = l_tmpl->InstanceTemplate();

		o_tmpl->SetInternalFieldCount(1);
		tmpl = Persistent<FunctionTemplate>::New(l_tmpl);
		_prototypes[name] = tmpl;
	}
	/* start adding the properties for such descriptor */
	ender_descriptor_property_list(descriptor, _v8_element_property_to_js,
			&tmpl);
	/* TODO add the functions */

	/* get the parent descriptor and do the same */
	descriptor = ender_descriptor_parent(descriptor);
	if (descriptor)
	{
		Persistent<FunctionTemplate> ptmpl =
				_v8_element_descriptor_to_js(thiz, descriptor);
		/* inherit from its parent */
		tmpl->Inherit(ptmpl);
	}
	return tmpl;
}

static Handle<Object> _v8_element_to_js(Esvg_Scriptor_Js_V8 *thiz, Ender_Element *e)
{
	Ender_Descriptor *descriptor;
	HandleScope handle_scope;

	descriptor = ender_element_descriptor_get(e);
	Persistent<FunctionTemplate> tmpl = _v8_element_descriptor_to_js(thiz,
			descriptor);

	Handle<Function> f = tmpl->GetFunction();
	Handle<Object> obj = f->NewInstance();
	obj->SetInternalField(0, External::New(e));
	return handle_scope.Close(obj);
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
	thiz->svg = e;

	/* create the global object */
	Handle<ObjectTemplate> global = ObjectTemplate::New();
	/* add default implementation of the alert message */
	global->Set(String::New("alert"), FunctionTemplate::New(Alert, External::New(thiz)));
	context = Context::New(NULL, global);
	/* enter the context so all the following operation are done there */
	context->Enter();

	/* store the context */
	thiz->context = context;

	/* add the svg document */
	context->Global()->Set(String::New("document"), _v8_element_to_js(thiz, e));

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

