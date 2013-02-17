
enesim_image_svg_LTLIBRARIES = src/modules/svg/enesim_image/enesim_image_svg.la
enesim_image_svgdir = $(pkglibdir)/enesim_image

src_modules_svg_enesim_image_enesim_image_svg_la_SOURCES = \
src/modules/svg/enesim_image/emage_svg.c

src_modules_svg_enesim_image_enesim_image_svg_la_CPPFLAGS = \
-I$(top_srcdir)/ecss/lib \
-I$(top_srcdir)/edom/lib \
-I$(top_srcdir)/esvg/lib \
-I$(top_srcdir)/esvg/lib/types \
-I$(top_srcdir)/esvg/lib/elements \
-DESVG_BUILD \
@ESVG_CFLAGS@

src_modules_svg_enesim_image_enesim_image_svg_la_LIBADD = src/lib/svg/libesvg.la @ESVG_LIBS@

src_modules_svg_enesim_image_enesim_image_svg_la_LDFLAGS = -no-undefined -module -avoid-version

src_modules_svg_enesim_image_enesim_image_svg_la_LIBTOOLFLAGS = --tag=disable-static
