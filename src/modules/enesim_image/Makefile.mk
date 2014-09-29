if BUILD_EGUEB_DOM

enesim_image_dom_LTLIBRARIES = src/modules/enesim_image/enesim_image_dom.la
enesim_image_domdir = @ENESIM_MODULE_IMAGE_DIR@

src_modules_enesim_image_enesim_image_dom_la_SOURCES = \
src/modules/enesim_image/enesim_image_dom.c

src_modules_enesim_image_enesim_image_dom_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib/css \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
-I$(top_srcdir)/src/lib/dom \
@EGUEB_DOM_CFLAGS@

src_modules_enesim_image_enesim_image_dom_la_LIBADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
@EGUEB_DOM_LIBS@

src_modules_enesim_image_enesim_image_dom_la_LDFLAGS = -no-undefined -module -avoid-version

src_modules_enesim_image_enesim_image_dom_la_LIBTOOLFLAGS = --tag=disable-static

endif
