descriptionsdir = @ENDER_DATADIR@
descriptions_DATA =

if BUILD_EGUEB_DOM
descriptions_DATA += data/egueb_dom.ender
endif

EXTRA_DIST += $(descriptions_DATA)
