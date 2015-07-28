descriptionsdir = @ENDER_DESCRIPTIONS_DIR@
descriptions_DATA =

if BUILD_EGUEB_DOM
descriptions_DATA += data/egueb-dom.ender
endif

if BUILD_EGUEB_SVG
descriptions_DATA += data/egueb-svg.ender
endif

if BUILD_EGUEB_SMIL
descriptions_DATA += data/egueb-smil.ender
endif

ender: doxygen
	@xsltproc --param lib "'egueb-dom'" --param version 0 --param case "'underscore'" @ENDER_TOOLS_DIR@/fromdoxygen.xslt $(top_builddir)/doc/xml/Egueb__Dom_8h.xml > out.xml
	@xmllint -format out.xml > $(top_srcdir)/data/egueb-dom-main.ender
	@xsltproc --param tomerge "'$(abs_top_srcdir)/data/egueb-dom-callbacks.ender'" @ENDER_TOOLS_DIR@/merge.xslt $(top_srcdir)/data/egueb-dom-main.ender > out.xml
	@xmllint -format out.xml > $(top_srcdir)/data/egueb-dom.ender
	@rm out.xml
	@xsltproc --param lib "'egueb-svg'" --param version 0 --param case "'underscore'" @ENDER_TOOLS_DIR@/fromdoxygen.xslt $(top_builddir)/doc/xml/Egueb__Svg_8h.xml > out.xml
	@xmllint -format out.xml > $(top_srcdir)/data/egueb-svg.ender
	@rm out.xml
	@xsltproc --param lib "'egueb-smil'" --param version 0 --param case "'underscore'" @ENDER_TOOLS_DIR@/fromdoxygen.xslt $(top_builddir)/doc/xml/Egueb__Smil_8h.xml > out.xml
	@xmllint -format out.xml > $(top_srcdir)/data/egueb-smil.ender
	@rm out.xml
	@xsltproc --param lib "'egueb-css'" --param version 0 --param case "'underscore'" @ENDER_TOOLS_DIR@/fromdoxygen.xslt $(top_builddir)/doc/xml/Egueb__Css_8h.xml > out.xml
	@xmllint -format out.xml > $(top_srcdir)/data/egueb-css.ender
	@rm out.xml

EXTRA_DIST += $(descriptions_DATA)
