
if BUILD_ESVG_SCRIPTOR_V8
src_lib_svg_libesvg_la_SOURCES += src/lib/svg/script_providers/esvg_script_provider_js_v8.cc
src_lib_svg_libesvg_la_LIBADD += @V8_LIBS@
endif

