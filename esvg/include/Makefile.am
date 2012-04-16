MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = generated

include_HEADERS = Esvg.h Esvg_Parser.h

ESVG_APIHEADERS = \
esvg_main.h \
esvg_element.h \
esvg_rect.h \
esvg_renderable.h \
esvg_svg.h \
esvg_types.h

EXTRA_DIST = \
esvg_private_main.h \
esvg_private_element.h

installed_apiheadersdir = $(prefix)/include/esvg
installed_apiheaders_DATA = $(ESVG_API_HEADERS)

