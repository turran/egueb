if ENS_ENABLE_TESTS

noinst_LTLIBRARIES += src/tests/libmydom.la

src_tests_libmydom_la_SOURCES = \
src/tests/egueb_dom_mydocument.c \
src/tests/egueb_dom_mydocument.h \
src/tests/egueb_dom_myelement.c \
src/tests/egueb_dom_myelement.h \
src/tests/egueb_dom_mylib.c \
src/tests/egueb_dom_mylib.h

src_tests_libmydom_la_CPPFLAGS = \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
$(src_lib_smil_libegueb_smil_la_IFLAGS) \
-I. \
@EGUEB_DOM_CFLAGS@

src_tests_libmydom_la_LIBADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
$(top_builddir)/src/lib/smil/libegueb_smil.la \
@EGUEB_DOM_LIBS@

tests_dom_CPPFLAGS = \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
@CHECK_CFLAGS@ \
@EGUEB_DOM_CFLAGS@

tests_dom_LDADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
$(top_builddir)/src/tests/libmydom.la \
@CHECK_LIBS@ \
@EGUEB_DOM_LIBS@

tests_css_CPPFLAGS = \
$(src_lib_css_libegueb_css_la_IFLAGS) \
$(tests_dom_CPPFLAGS) \
@CHECK_CFLAGS@ \
@EGUEB_CSS_CFLAGS@

tests_css_LDADD = \
$(top_builddir)/src/lib/css/libegueb_css.la \
$(tests_dom_LDADD) \
@CHECK_LIBS@ \
@EGUEB_CSS_LIBS@

tests_smil_CPPFLAGS = \
$(src_lib_smil_libegueb_smil_la_IFLAGS) \
$(tests_dom_CPPFLAGS) \
@CHECK_CFLAGS@ \
@EGUEB_SMIL_CFLAGS@

tests_smil_LDADD = \
$(top_builddir)/src/lib/smil/libegueb_smil.la \
$(tests_dom_LDADD) \
@CHECK_LIBS@ \
@EGUEB_SMIL_LIBS@

tests_svg_CPPFLAGS = \
$(src_lib_svg_libegueb_svg_la_IFLAGS) \
$(tests_dom_CPPFLAGS) \
$(tests_css_CPPFLAGS) \
$(tests_smil_CPPFLAGS) \
@CHECK_CFLAGS@ \
@EGUEB_SVG_CFLAGS@

tests_svg_LDADD = \
$(top_builddir)/src/lib/svg/libegueb_svg.la \
$(tests_dom_LDADD) \
$(tests_css_LDADD) \
$(tests_smil_LDADD) \
@CHECK_LIBS@ \
@EGUEB_SVG_LIBS@

local_TESTS = \
src/tests/egueb_dom_document01 \
src/tests/egueb_dom_document02 \
src/tests/egueb_dom_document03 \
src/tests/egueb_dom_document04 \
src/tests/egueb_dom_element01 \
src/tests/egueb_dom_element02 \
src/tests/egueb_dom_node01 \
src/tests/egueb_dom_node02 \
src/tests/egueb_svg_test01 \
src/tests/egueb_test_svg_animation01

check_PROGRAMS += $(local_TESTS)
TESTS += $(local_TESTS)

src_tests_egueb_dom_node01_SOURCES = src/tests/egueb_dom_node01.c
src_tests_egueb_dom_node01_CPPFLAGS = $(tests_dom_CPPFLAGS)
src_tests_egueb_dom_node01_LDADD = $(tests_dom_LDADD)

src_tests_egueb_dom_node02_SOURCES = src/tests/egueb_dom_node02.c
src_tests_egueb_dom_node02_CPPFLAGS = $(tests_dom_CPPFLAGS)
src_tests_egueb_dom_node02_LDADD = $(tests_dom_LDADD)

src_tests_egueb_dom_document01_SOURCES = src/tests/egueb_dom_document01.c
src_tests_egueb_dom_document01_CPPFLAGS = $(tests_dom_CPPFLAGS)
src_tests_egueb_dom_document01_LDADD = $(tests_dom_LDADD)

src_tests_egueb_dom_document02_SOURCES = src/tests/egueb_dom_document02.c
src_tests_egueb_dom_document02_CPPFLAGS = $(tests_dom_CPPFLAGS)
src_tests_egueb_dom_document02_LDADD = $(tests_dom_LDADD)

src_tests_egueb_dom_document03_SOURCES = src/tests/egueb_dom_document03.c
src_tests_egueb_dom_document03_CPPFLAGS = $(tests_dom_CPPFLAGS)
src_tests_egueb_dom_document03_LDADD = $(tests_dom_LDADD)

src_tests_egueb_dom_document04_SOURCES = src/tests/egueb_dom_document04.c
src_tests_egueb_dom_document04_CPPFLAGS = $(tests_dom_CPPFLAGS)
src_tests_egueb_dom_document04_LDADD = $(tests_dom_LDADD)

src_tests_egueb_dom_element01_SOURCES = src/tests/egueb_dom_element01.c
src_tests_egueb_dom_element01_CPPFLAGS = $(tests_dom_CPPFLAGS)
src_tests_egueb_dom_element01_LDADD = $(tests_dom_LDADD)

src_tests_egueb_dom_element02_SOURCES = src/tests/egueb_dom_element02.c
src_tests_egueb_dom_element02_CPPFLAGS = $(tests_dom_CPPFLAGS)
src_tests_egueb_dom_element02_LDADD = $(tests_dom_LDADD)

src_tests_egueb_svg_test01_SOURCES = src/tests/egueb_svg_test01.c
src_tests_egueb_svg_test01_CPPFLAGS = $(tests_svg_CPPFLAGS)
src_tests_egueb_svg_test01_LDADD = $(tests_svg_LDADD)

src_tests_egueb_test_svg_animation01_SOURCES = src/tests/egueb_test_svg_animation01.c
src_tests_egueb_test_svg_animation01_CPPFLAGS = $(tests_svg_CPPFLAGS)
src_tests_egueb_test_svg_animation01_LDADD = $(tests_svg_LDADD)

endif
