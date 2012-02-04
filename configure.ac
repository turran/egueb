# get rid of that stupid cache mechanism
rm -f config.cache

m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [15])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([egueb], [v_ver], [egueb-devel@googlegroups.com])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])
AC_DEFINE_UNQUOTED([VERSION_MAJOR], [v_maj], [Major version])
AC_DEFINE_UNQUOTED([VERSION_MINOR], [v_min], [Minor version])
AC_DEFINE_UNQUOTED([VERSION_MICRO], [v_mic], [Micro version])


### Needed information

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

EGUEB_BUILD=""
case "$host_os" in
   mingw*)
      EGUEB_BUILD="-DEGUEB_BUILD"
   ;;
esac
AC_SUBST([EGUEB_BUILD])


### Additional options to configure


### Checks for programs
AC_PROG_CC
PKG_PROG_PKG_CONFIG

# flex
AM_PROG_LEX
if test "x$ac_cv_prog_LEX" = "x"; then
  AC_MSG_ERROR(Cannot find flex)
fi

#yacc
AC_PROG_YACC
if test "x$ac_cv_prog_YACC" = "x"; then
  AC_MSG_ERROR(Cannot find bison)
fi

### Checks for libraries

PKG_CHECK_MODULES([EINA], [eina >= 1.0.99])
PKG_CHECK_MODULES(ENESIM, [enesim >= 0.0.12])
PKG_CHECK_MODULES(ETEX, [etex])

AC_MSG_CHECKING([whether emage support is available])
PKG_CHECK_MODULES([ESVG_EMAGE], [emage >= 0.0.4], [have_emage="yes"], [have_emage="no"])
AC_MSG_RESULT([${have_emage}])

if test "x${have_emage}" = "xyes" ; then
   AC_DEFINE(HAVE_EMAGE, [1], [Emage Support])
fi
AM_CONDITIONAL(BUILD_EMAGE, [test "x${have_emage}" = "xyes"])

### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C_BIGENDIAN
#EFL_ATTRIBUTE_UNUSED
#EFL_COMPILER_FLAG([-Wall])

### Checks for linker characteristics


### Checks for library functions

### Substitutions
EDOM_CFLAGS="$EINA_CFLAGS"
EDOM_LIBS="$EINA_LIBS"
AC_SUBST([EDOM_CFLAGS])
AC_SUBST([EDOM_LIBS])

ECSS_CFLAGS="$EINA_CFLAGS"
ECSS_LIBS="$EINA_LIBS"
AC_SUBST([ECSS_CFLAGS])
AC_SUBST([ECSS_LIBS])

ESVG_CFLAGS="$EINA_CFLAGS $ENESIM_CFLAGS -DENESIM_EXTENSION $ETEX_CFLAGS" 
ESVG_LIBS="$EINA_LIBS $ENESIM_LIBS $ETEX_LIBS" 
AC_SUBST([ESVG_CFLAGS])
AC_SUBST([ESVG_LIBS])

# FIXME make this use_foo conditional on the programs/headers/whatever found
use_dom="yes"
use_css="yes"
use_svg="yes"

## Make the debug preprocessor configurable

AC_OUTPUT([
edom/Makefile
edom/include/Makefile
edom/lib/Makefile
ecss/Makefile
ecss/tests/Makefile
ecss/include/Makefile
ecss/lib/Makefile
esvg/Makefile
esvg/bin/Makefile
esvg/lib/Makefile
esvg/lib/parser/Makefile
esvg/include/Makefile
esvg/modules/Makefile
esvg/modules/emage/Makefile
esvg/data/Makefile
esvg/data/ender/Makefile
Makefile
esvg.pc
ecss.pc
edom.pc
],[
])

#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "DOM support.................................: ${use_dom}"
echo "CSS support.................................: ${use_css}"
echo "SVG support.................................. ${use_svg}"
echo
echo "Installation Path...........................: ${prefix}"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
