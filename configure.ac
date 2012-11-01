# get rid of that stupid cache mechanism
rm -f config.cache

m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [18])
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

EINA_VERSION="1.0.99"
ENESIM_VERSION="0.0.16"
ETCH_VERSION="0.0.5"
ENDER_VERSION="0.0.7"

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

ECSS_BUILD=""
EDOM_BUILD=""
ESVG_BUILD=""
case "$host_os" in
   mingw*)
      ECSS_BUILD="-DECSS_BUILD"
      EDOM_BUILD="-DEDOM_BUILD"
      ESVG_BUILD="-DESVG_BUILD"
   ;;
esac
AC_SUBST([ECSS_BUILD])
AC_SUBST([EDOM_BUILD])
AC_SUBST([ESVG_BUILD])

ecss_requirements="eina"
edom_requirements="eina"
esvg_requirements="etex >= ${ENESIM_VERSION} enesim >= ${ENESIM_VERSION} eina >= ${EINA_VERSION} ender >= ${ENDER_VERSION} etch >= ${ETCH_VERSION}"
esvg_video_requirements="gstreamer-0.10"

AC_SUBST([esvg_requirements])

### Additional options to configure

AC_ARG_ENABLE([ecss],
   [AS_HELP_STRING([--enable-ecss], [enable Ecss compilation.])],
   [if test "x${enableval}" = "xyes"; then
       want_ecss="yes"
    else
       want_ecss="no"
    fi
   ],
   [want_ecss="yes"])

AM_CONDITIONAL([WANT_ECSS], [test "x${want_ecss}" = "xyes"])

AC_ARG_ENABLE([edom],
   [AS_HELP_STRING([--enable-edom], [enable Edom compilation.])],
   [if test "x${enableval}" = "xyes"; then
       want_edom="yes"
    else
       want_edom="no"
    fi
   ],
   [want_edom="yes"])

AM_CONDITIONAL([WANT_EDOM], [test "x${want_edom}" = "xyes"])

AC_ARG_ENABLE([esvg],
   [AS_HELP_STRING([--enable-esvg], [enable Esvg compilation.])],
   [if test "x${enableval}" = "xyes"; then
       want_esvg="yes"
    else
       want_esvg="no"
    fi
   ],
   [want_esvg="yes"])

AM_CONDITIONAL([WANT_ESVG], [test "x${want_esvg}" = "xyes"])


### Checks for programs

# compiler
AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST([pkgconfig_requires_private])

# flex (ecss dep)
if test "x${want_ecss}" = "xyes" ; then
   AM_PROG_LEX
   if test "x$ac_cv_prog_LEX" = "x"; then
      AC_MSG_WARN([Cannot find flex, Ecss compilation disabled])
      want_ecss="no"
   fi
fi

# yacc (ecss dep)

if test "x${want_ecss}" = "xyes" ; then
   AC_PROG_YACC
   if test "x$ac_cv_prog_YACC" = "x"; then
      AC_MSG_WARN([Cannot find bison, Ecss compilation disabled])
      want_ecss="no"
   fi
fi

### Checks for libraries

if test "x${want_ecss}" = "xyes" ; then
   PKG_CHECK_MODULES([ECSS], [${ecss_requirements}], [want_ecss="yes"], [want_ecss="no"])
fi

if test "x${want_edom}" = "xyes" ; then
   PKG_CHECK_MODULES([EDOM], [${edom_requirements}], [want_edom="yes"], [want_edom="no"])
   # ender is optional only for install time, not compile time
   PKG_CHECK_EXISTS([ender], [have_ender="yes"], [have_ender="no"])
   if test "x${have_ender}" = "xyes"; then
      ENDER_DATADIR=$($PKG_CONFIG --variable=datadir ender)
      AC_SUBST([ENDER_DATADIR])
   fi
fi

if test "x${want_ecss}" = "xyes" && test "x${want_edom}" = "xyes" ; then
   PKG_CHECK_MODULES([ESVG_EMAGE], [emage >= ${ENESIM_VERSION}], [have_emage="yes"], [have_emage="no"])
   AC_MSG_CHECKING([whether emage support is available])
   AC_MSG_RESULT([${have_emage}])

   if test "x${have_emage}" = "xyes" ; then
      AC_DEFINE(HAVE_EMAGE, [1], [Emage Support])
      esvg_requirements="emage >= ${ENESIM_VERSION} ${esvg_requirements}"
   fi
   PKG_CHECK_MODULES([ESVG], [${esvg_requirements}], [want_esvg="yes"], [want_esvg="no"])
   # Optional external tags
   PKG_CHECK_MODULES([ESVG_VIDEO], [${esvg_video_requirements}], [have_esvg_video="yes"], [have_esvg_video="no"])
   if test "x${have_esvg_video}" = "xyes" ; then
      AC_DEFINE(BUILD_ESVG_VIDEO, [1], [Esvg Video Support])
   fi
fi

AM_CONDITIONAL(BUILD_ECSS, [test "x${want_ecss}" = "xyes"])
AM_CONDITIONAL(BUILD_EDOM, [test "x${want_edom}" = "xyes"])
AM_CONDITIONAL(BUILD_EMAGE, [test "x${have_emage}" = "xyes"])
AM_CONDITIONAL(BUILD_ESVG, [test "x${want_esvg}" = "xyes"])
AM_CONDITIONAL(BUILD_ESVG_VIDEO, [test "x${have_esvg_video}" = "xyes"])

### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_C_O
AC_C_BIGENDIAN
#EFL_ATTRIBUTE_UNUSED
#EFL_COMPILER_FLAG([-Wall])

### Checks for linker characteristics


### Checks for library functions

## Make the debug preprocessor configurable

AC_CONFIG_FILES([
edom/Makefile
edom/include/Makefile
edom/lib/Makefile
edom/data/Makefile
ecss/Makefile
ecss/tests/Makefile
ecss/include/Makefile
ecss/lib/Makefile
esvg/Makefile
esvg/bin/Makefile
esvg/lib/Makefile
esvg/include/Makefile
esvg/include/generated/Makefile
esvg/modules/Makefile
esvg/modules/emage/Makefile
esvg/data/Makefile
Makefile
esvg.pc
ecss.pc
edom.pc
],[
])

AC_OUTPUT

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
echo "CSS support.................................: ${want_ecss}"
echo "DOM support.................................: ${want_edom}"
echo "SVG support.................................: ${want_esvg}"
echo "  External tags:"
echo "  video support (GStreamer).................: ${have_esvg_video}"
echo
echo "Installation Path...........................: ${prefix}"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
