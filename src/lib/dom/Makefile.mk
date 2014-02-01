
lib_LTLIBRARIES += src/lib/dom/libegueb_dom.la

installed_domheadersdir = $(pkgincludedir)-@VMAJ@/dom
dist_installed_domheaders_DATA = \
src/lib/dom/Egueb_Dom.h \
src/lib/dom/egueb_dom_attr.h \
src/lib/dom/egueb_dom_attr_external.h \
src/lib/dom/egueb_dom_attr_string.h \
src/lib/dom/egueb_dom_attr_string_list.h \
src/lib/dom/egueb_dom_build.h \
src/lib/dom/egueb_dom_character_data.h \
src/lib/dom/egueb_dom_document.h \
src/lib/dom/egueb_dom_document_external.h \
src/lib/dom/egueb_dom_element.h \
src/lib/dom/egueb_dom_element_external.h \
src/lib/dom/egueb_dom_event.h \
src/lib/dom/egueb_dom_event_external.h \
src/lib/dom/egueb_dom_event_mouse.h \
src/lib/dom/egueb_dom_event_mutation.h \
src/lib/dom/egueb_dom_event_ui.h \
src/lib/dom/egueb_dom_input.h \
src/lib/dom/egueb_dom_list.h \
src/lib/dom/egueb_dom_main.h \
src/lib/dom/egueb_dom_node.h \
src/lib/dom/egueb_dom_node_list.h \
src/lib/dom/egueb_dom_node_map_named.h \
src/lib/dom/egueb_dom_parser.h \
src/lib/dom/egueb_dom_string.h \
src/lib/dom/egueb_dom_string_list.h \
src/lib/dom/egueb_dom_text.h \
src/lib/dom/egueb_dom_uri.h \
src/lib/dom/egueb_dom_utils.h \
src/lib/dom/egueb_dom_value.h \
src/lib/dom/egueb_dom_value_dom_string.h \
src/lib/dom/egueb_dom_value_int.h \
src/lib/dom/egueb_dom_value_list.h

src_lib_dom_libegueb_dom_la_SOURCES = \
src/lib/dom/egueb_dom_attr.c \
src/lib/dom/egueb_dom_attr_external.c \
src/lib/dom/egueb_dom_attr_private.h \
src/lib/dom/egueb_dom_character_data.c \
src/lib/dom/egueb_dom_character_data_private.h \
src/lib/dom/egueb_dom_document.c \
src/lib/dom/egueb_dom_document_external.c \
src/lib/dom/egueb_dom_element.c \
src/lib/dom/egueb_dom_element_private.h \
src/lib/dom/egueb_dom_element_external.c \
src/lib/dom/egueb_dom_event.c \
src/lib/dom/egueb_dom_event_private.h \
src/lib/dom/egueb_dom_event_external.c \
src/lib/dom/egueb_dom_event_mouse.c \
src/lib/dom/egueb_dom_event_mouse_private.h \
src/lib/dom/egueb_dom_event_mutation.c \
src/lib/dom/egueb_dom_event_mutation_private.h \
src/lib/dom/egueb_dom_event_ui.c \
src/lib/dom/egueb_dom_event_ui_private.h \
src/lib/dom/egueb_dom_input.c \
src/lib/dom/egueb_dom_list.c \
src/lib/dom/egueb_dom_list_private.h \
src/lib/dom/egueb_dom_main.c \
src/lib/dom/egueb_dom_node.c \
src/lib/dom/egueb_dom_node_private.h \
src/lib/dom/egueb_dom_node_map_named.c \
src/lib/dom/egueb_dom_node_map_named_private.h \
src/lib/dom/egueb_dom_node_map_named_attr.c \
src/lib/dom/egueb_dom_parser.c \
src/lib/dom/egueb_dom_parser_private.h \
src/lib/dom/egueb_dom_attr_string.c \
src/lib/dom/egueb_dom_attr_string_list.c \
src/lib/dom/egueb_dom_attr_basic.c \
src/lib/dom/egueb_dom_attr_basic_private.h \
src/lib/dom/egueb_dom_attr_object.c \
src/lib/dom/egueb_dom_attr_object_private.h \
src/lib/dom/egueb_dom_attr_primitive.c \
src/lib/dom/egueb_dom_attr_primitive_private.h \
src/lib/dom/egueb_dom_private.h \
src/lib/dom/egueb_dom_string.c \
src/lib/dom/egueb_dom_string_list.c \
src/lib/dom/egueb_dom_text.c \
src/lib/dom/egueb_dom_uri.c \
src/lib/dom/egueb_dom_utils.c \
src/lib/dom/egueb_dom_value.c \
src/lib/dom/egueb_dom_value_private.h \
src/lib/dom/egueb_dom_value_dom_string.c \
src/lib/dom/egueb_dom_value_int.c \
src/lib/dom/egueb_dom_value_list.c

src_lib_dom_libegueb_dom_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/dom \
-DEGUEB_DOM_BUILD \
@EDOM_CFLAGS@

src_lib_dom_libegueb_dom_la_LIBADD = @EDOM_LIBS@

src_lib_dom_libegueb_dom_la_LDFLAGS = -no-undefined -version-info @version_info@
