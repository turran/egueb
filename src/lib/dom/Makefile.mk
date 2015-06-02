lib_LTLIBRARIES += src/lib/dom/libegueb_dom.la

installed_domheadersdir = $(pkgincludedir)-@VMAJ@/dom
dist_installed_domheaders_DATA = \
src/lib/dom/Egueb_Dom.h \
src/lib/dom/egueb_dom_attr.h \
src/lib/dom/egueb_dom_build.h \
src/lib/dom/egueb_dom_cdata_section.h \
src/lib/dom/egueb_dom_character_data.h \
src/lib/dom/egueb_dom_document.h \
src/lib/dom/egueb_dom_document_external.h \
src/lib/dom/egueb_dom_document_fragment.h \
src/lib/dom/egueb_dom_element.h \
src/lib/dom/egueb_dom_event.h \
src/lib/dom/egueb_dom_event_target.h \
src/lib/dom/egueb_dom_feature.h \
src/lib/dom/egueb_dom_implementation.h \
src/lib/dom/egueb_dom_input.h \
src/lib/dom/egueb_dom_list.h \
src/lib/dom/egueb_dom_main.h \
src/lib/dom/egueb_dom_node.h \
src/lib/dom/egueb_dom_node_list.h \
src/lib/dom/egueb_dom_node_map_named.h \
src/lib/dom/egueb_dom_parser.h \
src/lib/dom/egueb_dom_registry.h \
src/lib/dom/egueb_dom_scripter.h \
src/lib/dom/egueb_dom_string.h \
src/lib/dom/egueb_dom_string_list.h \
src/lib/dom/egueb_dom_text.h \
src/lib/dom/egueb_dom_uri.h \
src/lib/dom/egueb_dom_utils.h \
src/lib/dom/egueb_dom_value.h \
src/lib/dom/egueb_dom_media_provider.h \
src/lib/dom/egueb_dom_media_notifier.h \
src/lib/dom/egueb_dom_window.h

src_lib_dom_libegueb_dom_la_SOURCES = \
src/lib/dom/egueb_dom_attr.c \
src/lib/dom/egueb_dom_attr_private.h \
src/lib/dom/egueb_dom_cdata_section.c \
src/lib/dom/egueb_dom_character_data.c \
src/lib/dom/egueb_dom_character_data_private.h \
src/lib/dom/egueb_dom_document.c \
src/lib/dom/egueb_dom_document_private.h \
src/lib/dom/egueb_dom_document_external.c \
src/lib/dom/egueb_dom_document_fragment.c \
src/lib/dom/egueb_dom_element.c \
src/lib/dom/egueb_dom_element_private.h \
src/lib/dom/egueb_dom_event_target.c \
src/lib/dom/egueb_dom_event_target_private.h \
src/lib/dom/egueb_dom_event.c \
src/lib/dom/egueb_dom_event_private.h \
src/lib/dom/egueb_dom_feature.c \
src/lib/dom/egueb_dom_feature_private.h \
src/lib/dom/egueb_dom_implementation.c \
src/lib/dom/egueb_dom_input.c \
src/lib/dom/egueb_dom_list.c \
src/lib/dom/egueb_dom_list_private.h \
src/lib/dom/egueb_dom_main.c \
src/lib/dom/egueb_dom_node.c \
src/lib/dom/egueb_dom_node_private.h \
src/lib/dom/egueb_dom_node_list.c \
src/lib/dom/egueb_dom_node_list_private.h \
src/lib/dom/egueb_dom_node_map_named.c \
src/lib/dom/egueb_dom_node_map_named_private.h \
src/lib/dom/egueb_dom_node_map_named_attr.c \
src/lib/dom/egueb_dom_parser.c \
src/lib/dom/egueb_dom_parser_private.h \
src/lib/dom/egueb_dom_private.h \
src/lib/dom/egueb_dom_registry.c \
src/lib/dom/egueb_dom_registry_private.h \
src/lib/dom/egueb_dom_scripter.c \
src/lib/dom/egueb_dom_string.c \
src/lib/dom/egueb_dom_string_private.h \
src/lib/dom/egueb_dom_string_list.c \
src/lib/dom/egueb_dom_text.c \
src/lib/dom/egueb_dom_uri.c \
src/lib/dom/egueb_dom_utils.c \
src/lib/dom/egueb_dom_value.c \
src/lib/dom/egueb_dom_value_private.h \
src/lib/dom/egueb_dom_media_provider.c \
src/lib/dom/egueb_dom_media_notifier.c \
src/lib/dom/egueb_dom_window.c

src_lib_dom_libegueb_dom_la_IFLAGS = \
-I$(top_srcdir)/src/lib/dom \
-I$(top_srcdir)/src/lib/dom/attr \
-I$(top_srcdir)/src/lib/dom/element \
-I$(top_srcdir)/src/lib/dom/event \
-I$(top_srcdir)/src/lib/dom/feature \
-I$(top_srcdir)/src/lib/dom/value

src_lib_dom_libegueb_dom_la_CPPFLAGS = \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DEGUEB_DOM_BUILD \
@EGUEB_DOM_CFLAGS@

src_lib_dom_libegueb_dom_la_LIBADD = \
@EGUEB_DOM_LIBS@

src_lib_dom_libegueb_dom_la_LDFLAGS = -no-undefined -version-info @version_info@
