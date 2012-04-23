MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = generated

include_HEADERS = Esvg.h Esvg_Parser.h

ESVG_APIHEADERS = \
esvg_circle.h \
esvg_element.h \
esvg_ellipse.h \
esvg_g.h \
esvg_gradient.h \
esvg_line.h \
esvg_linear_gradient.h \
esvg_main.h \
esvg_path.h \
esvg_polygon.h \
esvg_polyline.h \
esvg_rect.h \
esvg_renderable.h \
esvg_stop.h \
esvg_svg.h \
esvg_types.h \
esvg_use.h

EXTRA_DIST = \
esvg_private_main.h \
esvg_private_element.h

installed_apiheadersdir = $(prefix)/include/esvg
installed_apiheaders_DATA = $(ESVG_APIHEADERS)

