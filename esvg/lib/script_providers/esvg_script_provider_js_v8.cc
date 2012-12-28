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
#include "Esvg.h"

#ifdef __cplusplus
extern "C" {
#endif

void esvg_script_provider_js_v8_init(void);
void esvg_script_provider_js_v8_shutdown(void);

#ifdef __cplusplus
}
#endif


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
#define ESVG_LOG_COLOR_DEFAULT EINA_COLOR_ORANGE
/* Whenever a file needs to generate a log, it must declare this first */

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_esvg_script_provider_js_v8_log, __VA_ARGS__)

#ifdef WARN
# undef WARN
#endif
#define WARN(...) EINA_LOG_DOM_WARN(_esvg_script_provider_js_v8_log, __VA_ARGS__)

#ifdef INFO
# undef INFO
#endif
#define INFO(...) EINA_LOG_DOM_INFO(_esvg_script_provider_js_v8_log, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_esvg_script_provider_js_v8_log, __VA_ARGS__)

static int _esvg_script_provider_js_v8_log = -1;

typedef struct _Esvg_Script_Provider_Js_V8
{
	Ender_Element *svg;
	Persistent<Context> context;
} Esvg_Script_Provider_Js_V8;

static Handle<Object> _v8_element_to_js(Esvg_Script_Provider_Js_V8 *thiz,
		Ender_Element *e);

/* FIXME or we either use the stl or an eina hash, but looks like
 * we cannot access the pointer of the persistent handle to pass
 * it to eina_hash?
 */
static std::map<const char *, Persistent<FunctionTemplate> > _prototypes;

static Ender_Value * _v8_value_to_ender(Handle<Value> v, Ender_Container *c)
{
	Ender_Value *vv = NULL;
	Ender_Value_Type vtype;

	vtype = ender_container_type_get(c);
	switch (vtype)
	{
		case ENDER_STRING:
		if (v->IsString())
		{
			String::Utf8Value str(v);
			vv = ender_value_basic_new(ENDER_STRING);
			ender_value_static_string_set(vv, *str);
		}
		break;

		case ENDER_BOOL:
		case ENDER_UINT32:
		case ENDER_INT32:
		case ENDER_UINT64:
		case ENDER_INT64:
		case ENDER_DOUBLE:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_OBJECT:
		case ENDER_ENDER:
		case ENDER_POINTER:
		case ENDER_VALUE:
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
		default:
		break;
	}
	return vv;
}

static Handle<Value> _v8_value_from_ender(Ender_Value *v)
{
	Ender_Value_Type vtype;
	Handle<Value> vv = Undefined();

	vtype = ender_value_type_get(v);
	switch (vtype)
	{
		case ENDER_STRING:
		case ENDER_BOOL:
		case ENDER_UINT32:
		case ENDER_INT32:
		case ENDER_UINT64:
		case ENDER_INT64:
		case ENDER_DOUBLE:
		case ENDER_COLOR:
		case ENDER_ARGB:
		case ENDER_OBJECT:
		break;

		case ENDER_ENDER:
		{
			Ender_Element *e = ender_value_ender_get(v);
			if (!e) break;
			Local<Object> global = Context::GetCurrent()->Global();
			Esvg_Script_Provider_Js_V8 *thiz = (Esvg_Script_Provider_Js_V8 *)global->GetPointerFromInternalField(1);
			vv = _v8_element_to_js(thiz, e);
		}
		case ENDER_POINTER:
		case ENDER_VALUE:
		case ENDER_LIST:
		case ENDER_STRUCT:
		case ENDER_UNION:
		default:
		break;
	}
	return vv;
}

static v8::Handle<v8::Value> _v8_alert(const v8::Arguments& args)
{
	Esvg_Script_Provider_Js_V8 *thiz;

	String::Utf8Value str(args[0]); // Convert first argument to V8 String
	Local<External> wrap = Local<External>::Cast(args.Data());
	thiz = static_cast<Esvg_Script_Provider_Js_V8*>(wrap->Value());

	/* call the svg with the string */
	esvg_svg_script_alert(thiz->svg, *str);
	return Undefined();
}

static Handle<Value> _v8_getter(Local<String> property, const AccessorInfo &info)
{
	Local<Object> self = info.Holder();
	Local<External> data = Local<External>::Cast(info.Data());
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	Ender_Element *e = static_cast<Ender_Element *>(wrap->Value());
	Ender_Property *p = static_cast<Ender_Property *>(data->Value());

	DBG("Getting '%s' property", ender_property_name_get(p));
	ender_element_property_value_get_simple(e, p, NULL);
	return Undefined();
}

static void _v8_setter(Local<String> property, Local<Value> value,
		const AccessorInfo& info)
{
	Local<Object> self = info.Holder();
	Local<External> data = Local<External>::Cast(info.Data());
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	Ender_Element *e = static_cast<Ender_Element *>(wrap->Value());
	Ender_Property *p = static_cast<Ender_Property *>(data->Value());

	DBG("Setting '%s' property", ender_property_name_get(p));
	ender_element_property_value_set_simple(e, p, NULL);
}

static v8::Handle<v8::Value> _v8_function(const v8::Arguments& args)
{
	Local<Object> self = args.Holder();
	Local<External> data = Local<External>::Cast(args.Data());
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	Ender_Element *e = static_cast<Ender_Element *>(wrap->Value());
	Ender_Function *f = static_cast<Ender_Function *>(data->Value());
	Ender_Value *ret = NULL;
	Eina_List *vargs = NULL;
	Eina_List *l;

	/* check that we have the correct number of arguments */
	if (args.Length() != ender_function_args_count(f))
	{
		ERR("Wrong number of arguments for function '%s'"
				"%d %d", ender_function_name_get(f),
				args.Length(),
				ender_function_args_count(f));
		return Undefined();
	}

	/* get the return value */
	Ender_Container *c = ender_function_ret_get(f);
	if (c)
	{
		ret = ender_value_new_container_from(c);
	}
	/* get the function args */
	Eina_List *fargs = (Eina_List *)ender_function_args_get(f);
	for (int i = 0; i < args.Length(); i++)
	{
		Ender_Value *v;
		c = (Ender_Container *)fargs->data;

		v = _v8_value_to_ender(args[i], c);
		/* TODO check that the container is the same */
		fargs = fargs->next;
		vargs = eina_list_append(vargs, v);
	}

	DBG("Calling function '%s' on %p", ender_function_name_get(f), e);
	ender_element_function_call_simple(e, f, ret, vargs);
	/* transform the return value */
	if (ret)
	{
		return _v8_value_from_ender(ret);
	}
	else
	{
		return Undefined();
	}
}

static void _v8_element_function_to_js(Ender_Function *f,
		void *data)
{
	Ender_Container *c;
	const char *name = (const char *)data;
	const char *fname;

	fname = ender_function_name_get(f);
	DBG("Adding function '%s'", fname);
	Persistent<FunctionTemplate> tmpl = _prototypes[name];
	HandleScope handle_scope;
	Handle<ObjectTemplate> p_tmpl = tmpl->PrototypeTemplate();
	p_tmpl->Set(String::New(fname), FunctionTemplate::New(_v8_function,
			External::New(f)));
}

static void _v8_element_property_to_js(Ender_Property *prop,
		void *data)
{
	Ender_Container *c;
	const char *name = (const char *)data;
	const char *pname;

#if 0
	Persistent<FunctionTemplate> tmpl = Persistent<FunctionTemplate>::New(External::Wrap(data));
#endif
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
	pname = ender_property_name_get(prop);
	DBG("Adding property '%s'", pname);
	/* add an accessor to the template */
	HandleScope handle_scope;
	Handle<ObjectTemplate> p_tmpl = tmpl->PrototypeTemplate();
	/* TODO we should not support setters */
	p_tmpl->SetAccessor(String::New(pname), _v8_getter, _v8_setter,
			External::New(prop));
}

static Persistent<FunctionTemplate> _v8_element_descriptor_to_js(
		Esvg_Script_Provider_Js_V8 *thiz,
		Ender_Descriptor *descriptor)
{
	const char *name;
	void *cb_data;

	/* find the descriptor name in the hash of templates */
	name = ender_descriptor_name_get(descriptor);
	Persistent<FunctionTemplate> tmpl = _prototypes[name];
	if (!tmpl.IsEmpty()) return tmpl;

	DBG("Adding descriptor '%s'", ender_descriptor_name_get(descriptor));
	/* create a new prototype and store it */
	HandleScope handle_scope;
	Handle<FunctionTemplate> l_tmpl = FunctionTemplate::New();
	l_tmpl->SetClassName(String::New(name));
	Handle<ObjectTemplate> o_tmpl = l_tmpl->InstanceTemplate();

	o_tmpl->SetInternalFieldCount(1);
	tmpl = Persistent<FunctionTemplate>::New(l_tmpl);
	_prototypes[name] = tmpl;
	/* start adding the properties for such descriptor */
#if 0
	cb_data = static_cast<void *>(External::Unwrap(tmpl));
#endif
	ender_descriptor_property_list(descriptor, _v8_element_property_to_js,
			(void *)name);
	ender_descriptor_function_list(descriptor, _v8_element_function_to_js,
			(void *)name);
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

static Handle<Object> _v8_element_to_js(Esvg_Script_Provider_Js_V8 *thiz,
		Ender_Element *e)
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

/*----------------------------------------------------------------------------*
 *                          The script  interface                             *
 *----------------------------------------------------------------------------*/
static void * _v8_context_new(Ender_Element *e)
{
	Esvg_Script_Provider_Js_V8 *thiz;
	HandleScope handle_scope;
	Persistent<Context> context;

	thiz = (Esvg_Script_Provider_Js_V8 *)calloc(1, sizeof(Esvg_Script_Provider_Js_V8));
	thiz->svg = e;

	/* create the global object */
	Handle<ObjectTemplate> global = ObjectTemplate::New();
	/* add default implementation of the alert message */
	global->Set(String::New("alert"), FunctionTemplate::New(_v8_alert, External::New(thiz)));
	global->SetInternalFieldCount(1);
	context = Context::New(NULL, global);
	context->Global()->SetPointerInInternalField(0, thiz);
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
	Esvg_Script_Provider_Js_V8 *thiz = (Esvg_Script_Provider_Js_V8 *)ctx;

	thiz->context.Dispose();
	free(thiz);
}

static void _v8_run(void *ctx, const char *s)
{
	Esvg_Script_Provider_Js_V8 *thiz = (Esvg_Script_Provider_Js_V8 *)ctx;
	HandleScope handle_scope;

	/* create the source string */
	Handle<String> source = String::New(s);
	/* compile it */
	Handle<Script> script = Script::Compile(source);
	/* run it */
 	script->Run();
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_script_provider_js_v8_init(void)
{
	_esvg_script_provider_js_v8_log = eina_log_domain_register(
			"esvg_script_provider_js_v8", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_script_provider_js_v8_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
}

void esvg_script_provider_js_v8_shutdown(void)
{
	if (_esvg_script_provider_js_v8_log < 0)
		return;
	eina_log_domain_unregister(_esvg_script_provider_js_v8_log);
	_esvg_script_provider_js_v8_log = -1;
}

Esvg_Script_Provider_Descriptor esvg_script_provider_js_v8_descriptor = {
	/* .context_new 	= */ _v8_context_new,
	/* .context_free	= */ _v8_context_free,
	/* .run 		= */ _v8_run,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

