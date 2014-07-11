descriptionsdir = @ENDER_DATADIR@
descriptions_DATA =

if BUILD_EGUEB_DOM
descriptions_DATA += data/egueb_dom.ender
endif

if BUILD_EGUEB_SVG
descriptions_DATA += data/egueb_svg.ender
endif

EXTRA_DIST += $(descriptions_DATA)
