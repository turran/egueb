lib_LTLIBRARIES += src/lib/smil/libegueb_smil.la

installed_smilheadersdir = $(pkgincludedir)-@VMAJ@/smil
dist_installed_smilheaders_DATA = \
src/lib/smil/Egueb_Smil.h \
src/lib/smil/egueb_smil_accumulate.h \
src/lib/smil/egueb_smil_additive.h \
src/lib/smil/egueb_smil_animate_base.h \
src/lib/smil/egueb_smil_animate.h \
src/lib/smil/egueb_smil_animation.h \
src/lib/smil/egueb_smil_attribute_type.h \
src/lib/smil/egueb_smil_build.h \
src/lib/smil/egueb_smil_calc_mode.h \
src/lib/smil/egueb_smil_clock.h \
src/lib/smil/egueb_smil_duration.h \
src/lib/smil/egueb_smil_event.h \
src/lib/smil/egueb_smil_event_timeline.h \
src/lib/smil/egueb_smil_feature_animation.h \
src/lib/smil/egueb_smil_fill.h \
src/lib/smil/egueb_smil_main.h \
src/lib/smil/egueb_smil_repeat_count.h \
src/lib/smil/egueb_smil_set.h \
src/lib/smil/egueb_smil_timing.h

src_lib_smil_libegueb_smil_la_SOURCES = \
src/lib/smil/egueb_smil_private.h \
src/lib/smil/egueb_smil_accumulate.c \
src/lib/smil/egueb_smil_accumulate_private.h \
src/lib/smil/egueb_smil_additive.c \
src/lib/smil/egueb_smil_additive_private.h \
src/lib/smil/egueb_smil_animation.c \
src/lib/smil/egueb_smil_animation_private.h \
src/lib/smil/egueb_smil_animate.c \
src/lib/smil/egueb_smil_animate_base.c \
src/lib/smil/egueb_smil_animate_base_private.h \
src/lib/smil/egueb_smil_calc_mode.c \
src/lib/smil/egueb_smil_calc_mode_private.h \
src/lib/smil/egueb_smil_clock.c \
src/lib/smil/egueb_smil_clock_private.h \
src/lib/smil/egueb_smil_duration.c \
src/lib/smil/egueb_smil_duration_private.h \
src/lib/smil/egueb_smil_event.c \
src/lib/smil/egueb_smil_event_timeline.c \
src/lib/smil/egueb_smil_event_timeline_private.h \
src/lib/smil/egueb_smil_feature_animation.c \
src/lib/smil/egueb_smil_fill.c \
src/lib/smil/egueb_smil_fill_private.h \
src/lib/smil/egueb_smil_keyframe_private.h \
src/lib/smil/egueb_smil_key_spline.c \
src/lib/smil/egueb_smil_key_spline_private.h \
src/lib/smil/egueb_smil_main.c \
src/lib/smil/egueb_smil_repeat_count.c \
src/lib/smil/egueb_smil_repeat_count_private.h \
src/lib/smil/egueb_smil_set.c \
src/lib/smil/egueb_smil_signal.c \
src/lib/smil/egueb_smil_signal_private.h \
src/lib/smil/egueb_smil_signal_continuous.c \
src/lib/smil/egueb_smil_signal_discrete.c \
src/lib/smil/egueb_smil_timeline.c \
src/lib/smil/egueb_smil_timeline_private.h \
src/lib/smil/egueb_smil_timing.c \
src/lib/smil/egueb_smil_timing_private.h \
src/lib/smil/egueb_smil_timing_list.c \
src/lib/smil/egueb_smil_timing_list_private.h

src_lib_smil_libegueb_smil_la_IFLAGS = \
-I$(top_srcdir)/src/lib/smil \
-I$(top_srcdir)/src/lib/smil/value \
-I$(top_srcdir)/src/lib/smil/attr

src_lib_smil_libegueb_smil_la_CPPFLAGS = \
$(src_lib_smil_libegueb_smil_la_IFLAGS) \
$(src_lib_dom_libegueb_dom_la_IFLAGS) \
-DEGUEB_SMIL_BUILD \
@EGUEB_SMIL_CFLAGS@

src_lib_smil_libegueb_smil_la_LIBADD = \
$(top_builddir)/src/lib/dom/libegueb_dom.la \
@EGUEB_SMIL_LIBS@

src_lib_smil_libegueb_smil_la_LDFLAGS = -no-undefined -version-info @version_info@

