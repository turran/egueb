
if BUILD_ESVG_VIDEO_GSTREAMER
src_lib_svg_libesvg_la_CPPFLAGS += @GSTREAMER_CFLAGS@
src_lib_svg_libesvg_la_SOURCES += src/lib/svg/video_providers/esvg_video_provider_gstreamer.c
src_lib_svg_libesvg_la_LIBADD += @GSTREAMER_LIBS@
endif
