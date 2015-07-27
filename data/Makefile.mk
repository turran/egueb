descriptionsdir = @ENDER_DESCRIPTIONS_DIR@
descriptions_DATA =

if BUILD_EGUEB_DOM
descriptions_DATA += data/egueb_dom.ender
endif

if BUILD_EGUEB_SVG
descriptions_DATA += data/egueb_svg.ender
endif

if BUILD_EGUEB_SMIL
descriptions_DATA += data/egueb_smil.ender
endif

ender: doxygen
	@xsltproc --param lib "'egueb_dom'" --param version 0 --param case "'underscore'" @ENDER_TOOLS_DIR@/fromdoxygen.xslt $(top_builddir)/doc/xml/Egueb__Dom_8h.xml > out.xml
	@xmllint -format out.xml > $(top_srcdir)/data/egueb_dom.ender
	@rm out.xml
	@xsltproc --param lib "'egueb_svg'" --param version 0 --param case "'underscore'" @ENDER_TOOLS_DIR@/fromdoxygen.xslt $(top_builddir)/doc/xml/Egueb__Svg_8h.xml > out.xml
	@xmllint -format out.xml > $(top_srcdir)/data/egueb_svg.ender
	@rm out.xml
	@xsltproc --param lib "'egueb_smil'" --param version 0 --param case "'underscore'" @ENDER_TOOLS_DIR@/fromdoxygen.xslt $(top_builddir)/doc/xml/Egueb__Smil_8h.xml > out.xml
	@xmllint -format out.xml > $(top_srcdir)/data/egueb_smil.ender
	@rm out.xml

EXTRA_DIST += $(descriptions_DATA)
