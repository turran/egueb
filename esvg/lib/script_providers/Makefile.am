MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/ecss/lib \
-I$(top_srcdir)/edom/lib \
-I$(top_srcdir)/esvg/lib \
-DENESIM_EXTENSION \
-DESVG_BUILD \
@ESVG_CFLAGS@

noinst_LTLIBRARIES = libesvg_script_providers.la

libesvg_script_providers_la_SOURCES =
libesvg_script_providers_la_LIBADD =

if BUILD_ESVG_SCRIPTOR_V8
libesvg_script_providers_la_SOURCES += esvg_script_provider_js_v8.cc
libesvg_script_providers_la_LIBADD += @V8_LIBS@
endif

