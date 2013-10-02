# get rid of that stupid cache mechanism
rm -f config.cache

m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [22])
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

## Add parameters for aclocal (keep in sync with Makefile.am)
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4 -I common/m4")

# compiler
AC_PROG_CC
AC_PROG_CXX

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])
AC_DEFINE_UNQUOTED([VERSION_MAJOR], [v_maj], [Major version])
AC_DEFINE_UNQUOTED([VERSION_MINOR], [v_min], [Minor version])
AC_DEFINE_UNQUOTED([VERSION_MICRO], [v_mic], [Micro version])

VMAJ=v_maj
AC_SUBST([VMAJ])

### Needed information

EINA_VERSION="1.0.99"
EINA_EXTRA_VERSION="0.0.1"
ENESIM_VERSION="0.0.18"
ETCH_VERSION="0.0.5"

AC_CANONICAL_HOST

ecss_requirements="eina >= ${EINA_VERSION}"
edom_requirements="etch >= ${ETCH_VERSION} enesim >= ${ENESIM_VERSION} eina-extra >= ${EINA_EXTRA_VERSION} eina >= ${EINA_VERSION}"
esmil_requirements="etch >= ${ETCH_VERSION} enesim >= ${ENESIM_VERSION} eina-extra >= ${EINA_EXTRA_VERSION}  eina >= ${EINA_VERSION}"
esvg_requirements="etch >= ${ETCH_VERSION} enesim >= ${ENESIM_VERSION} eina-extra >= ${EINA_EXTRA_VERSION} eina >= ${EINA_VERSION}"
gstreamer_requirements="gstreamer-0.10"

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

AC_ARG_ENABLE([esmil],
   [AS_HELP_STRING([--enable-esmil], [enable Esmil compilation.])],
   [if test "x${enableval}" = "xyes"; then
       want_esmil="yes"
    else
       want_esmil="no"
    fi
   ],
   [want_esmil="yes"])

AM_CONDITIONAL([WANT_ESMIL], [test "x${want_esmil}" = "xyes"])

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

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if ! $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   AC_MSG_ERROR([pkg-config >= 0.22 is required])
fi

ENESIM_MODULE_DIR=`${PKG_CONFIG} --variable=pkglibdir enesim`
AC_SUBST([ENESIM_MODULE_DIR])

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

# doxygen program for documentation building
ENS_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

### Checks for libraries

if test "x${want_ecss}" = "xyes" ; then
   PKG_CHECK_MODULES([ECSS], [${ecss_requirements}], [want_ecss="yes"], [want_ecss="no"])
fi

if test "x${want_edom}" = "xyes" ; then
   PKG_CHECK_MODULES([EDOM], [${edom_requirements}], [want_edom="yes"], [want_edom="no"])
fi

if test "x${want_esmil}" = "xyes" && test "x${want_edom}" = "xyes" ; then
   PKG_CHECK_MODULES([ESMIL], [${esmil_requirements}], [want_esmil="yes"], [want_esmil="no"])
fi

if test "x${want_esvg}" = "xyes" && test "x${want_ecss}" = "xyes" && test "x${want_edom}" = "xyes" && test "x${want_esmil}" = "xyes" ; then
   PKG_CHECK_MODULES([ESVG], [${esvg_requirements}], [want_esvg="yes"], [want_esvg="no"])
   # Video backends
   PKG_CHECK_MODULES([GSTREAMER], [${gstreamer_requirements}], [have_gstreamer="yes"], [have_gstreamer="no"])
   if test "x${have_gstreamer}" = "xyes" ; then
      AC_DEFINE(BUILD_ESVG_VIDEO_GSTREAMER, [1], [Esvg Video Gstreamer Support])
   fi
   # Script backends
   # V8 code taken from elev8
   # Check for libv8 and v8.h by linking a bit of code using v8
   AC_LANG_PUSH(C++)
   AC_SUBST(V8_LIBS)
   AC_MSG_CHECKING([for libv8])
   saved_LIBS="$LIBS"
   LIBS="$LIBS -lv8"
   AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
     [[
      #include <v8.h>
      v8::Persistent<v8::Number> n;
     ]],
     [[
      n = v8::Persistent<v8::Number>::New(v8::Number::New(0));
      v8::Handle<v8::Object> o = v8::Object::New();
     ]],
   )],[[V8_LIBS="-lv8"]
      [have_v8="yes"]], [have_v8="no"])
   LIBS="$saved_LIBS"
   AC_MSG_RESULT([${have_v8}])
   AC_LANG_POP
fi

# For Dom test
PKG_CHECK_MODULES([ETCH], [etch])

AM_CONDITIONAL(BUILD_ECSS, [test "x${want_ecss}" = "xyes"])
AM_CONDITIONAL(BUILD_EDOM, [test "x${want_edom}" = "xyes"])
AM_CONDITIONAL(BUILD_ESMIL, [test "x${want_esmil}" = "xyes"])
AM_CONDITIONAL(BUILD_ESVG, [test "x${want_esvg}" = "xyes"])
AM_CONDITIONAL(BUILD_ESVG_VIDEO_GSTREAMER, [test "x${have_gstreamer}" = "xyes"])

if test "x${have_v8}" = "xyes" ; then
  AC_DEFINE(BUILD_ESVG_SCRIPTOR_V8, [1], [JS Scripting V8 Support])
fi
AM_CONDITIONAL(BUILD_ESVG_SCRIPTOR_V8, [test "x${have_v8}" = "xyes"])

### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_C_O
AC_C_BIGENDIAN
#EFL_ATTRIBUTE_UNUSED
EFL_COMPILER_FLAG([-Wall])

### Checks for linker characteristics


### Checks for library functions

## Make the debug preprocessor configurable

AC_CONFIG_FILES([
Makefile
doc/Doxyfile
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
echo "SMIL support................................: ${want_esmil}"
echo "SVG support.................................: ${want_esvg}"
echo "  Video support:"
echo "    GStreamer...............................: ${have_gstreamer}"
echo "  Scripting support:"
echo "    V8......................................: ${have_v8}"
echo
echo "Installation Path...........................: ${prefix}"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
