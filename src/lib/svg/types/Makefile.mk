GENERATED_FILES += \
src/lib/svg/types/esvg_generated_boolean_animated.c \
src/lib/svg/types/esvg_generated_color.c \
src/lib/svg/types/esvg_generated_color_animated.c \
src/lib/svg/types/esvg_generated_duration.c \
src/lib/svg/types/esvg_generated_length.c \
src/lib/svg/types/esvg_generated_length_animated.c \
src/lib/svg/types/esvg_generated_matrix.c \
src/lib/svg/types/esvg_generated_matrix_animated.c \
src/lib/svg/types/esvg_generated_number.c \
src/lib/svg/types/esvg_generated_number_animated.c \
src/lib/svg/types/esvg_generated_paint.c \
src/lib/svg/types/esvg_generated_paint_animated.c \
src/lib/svg/types/esvg_generated_point.c \
src/lib/svg/types/esvg_generated_rect.c \
src/lib/svg/types/esvg_generated_rect_animated.c \
src/lib/svg/types/esvg_generated_string_animated.c

esvg_generated_matrix_GFLAGS = -o
esvg_generated_rect_GFLAGS = -o

dist_installed_svgheaders_DATA += \
src/lib/svg/types/esvg_boolean.h \
src/lib/svg/types/esvg_color.h \
src/lib/svg/types/esvg_length.h \
src/lib/svg/types/esvg_matrix.h \
src/lib/svg/types/esvg_number.h \
src/lib/svg/types/esvg_paint.h \
src/lib/svg/types/esvg_rect.h \
src/lib/svg/types/esvg_string.h

src_lib_svg_libesvg_la_SOURCES += \
src/lib/svg/types/esvg_boolean.c \
src/lib/svg/types/esvg_color.c \
src/lib/svg/types/esvg_duration.c \
src/lib/svg/types/esvg_length.c \
src/lib/svg/types/esvg_matrix.c \
src/lib/svg/types/esvg_number.c \
src/lib/svg/types/esvg_paint.c \
src/lib/svg/types/esvg_point.c \
src/lib/svg/types/esvg_rect.c \
src/lib/svg/types/esvg_string.c
