CLEANFILES += \
src/lib/css/engine/egueb_css_engine_lexer.c \
src/lib/css/engine/egueb_css_engine_grammar.c \
src/lib/css/engine/egueb_css_engine_grammar.h

src_lib_css_libegueb_css_la_SOURCES += \
src/lib/css/engine/egueb_css_engine_context.c \
src/lib/css/engine/egueb_css_engine_context_private.h \
src/lib/css/engine/egueb_css_engine_parser_private.h \
src/lib/css/engine/egueb_css_engine_node.c \
src/lib/css/engine/egueb_css_engine_node_private.h \
src/lib/css/engine/egueb_css_engine_rule.c \
src/lib/css/engine/egueb_css_engine_rule_private.h \
src/lib/css/engine/egueb_css_engine_selector.c \
src/lib/css/engine/egueb_css_engine_selector_private.h \
src/lib/css/engine/egueb_css_engine_style.c \
src/lib/css/engine/egueb_css_engine_style_private.h \
src/lib/css/engine/egueb_css_engine_grammar.y \
src/lib/css/engine/egueb_css_engine_filter.c \
src/lib/css/engine/egueb_css_engine_filter_class.c \
src/lib/css/engine/egueb_css_engine_filter_id.c \
src/lib/css/engine/egueb_css_engine_filter_private.h \
src/lib/css/engine/egueb_css_engine_lexer.l \
src/lib/css/engine/src_lib_css_libegueb_css_la-egueb_css_engine_grammar.h

src_lib_css_libegueb_css_la_YFLAGS = -d -p egueb_css_engine_
src_lib_css_libegueb_css_la_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

src_lib_css_libegueb_css_la_LIBADD += \
@LEXLIB@
