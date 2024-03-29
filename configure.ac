dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.62)
AC_INIT([mate-calc], [1.28.0], [https://github.com/mate-desktop/mate-calc/issues],
        [mate-calc], [https://mate-desktop.org])

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-xz check-news])
AC_CONFIG_HEADERS(config.h)
AM_MAINTAINER_MODE
MATE_MAINTAINER_MODE_DEFINES
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_ISC_POSIX
AC_PROG_CC
AC_HEADER_STDC

GLIB_GSETTINGS
MATE_COMPILE_WARNINGS

dnl ###########################################################################
dnl Dependencies
dnl ###########################################################################

GLIB_REQUIRED=2.50.0
GIO_REQUIRED=2.36.0
GTK_REQUIRED=3.22.0
MPFR_REQUIRED=4.0.2

PKG_CHECK_MODULES(MATE_CALC, [
    gtk+-3.0 >= $GTK_REQUIRED
    glib-2.0 >= $GLIB_REQUIRED
    gio-2.0 >= $GIO_REQUIRED
    mpfr >= $MPFR_REQUIRED
    libxml-2.0
    gmodule-export-2.0
])

PKG_CHECK_MODULES(MATE_CALC_CMD, [
    glib-2.0 >= $GLIB_REQUIRED
    gio-2.0 >= $GIO_REQUIRED
    mpfr >= $MPFR_REQUIRED
    libxml-2.0
])

GLIB_MKENUMS=`$PKG_CONFIG --variable=glib_mkenums glib-2.0`
AC_SUBST(GLIB_MKENUMS)

AC_CHECK_LIB(m, log)
AC_CHECK_LIB(mpc, log, [], [AC_MSG_ERROR(could not find required development libraries for MPC)], [])

dnl ###########################################################################
dnl Internationalization
dnl ###########################################################################
AM_GNU_GETTEXT_VERSION([0.19.8])
AM_GNU_GETTEXT_REQUIRE_VERSION([0.19.8])
AM_GNU_GETTEXT([external])

GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE",[the gettext translation domain])

# **********************
# GLIB_COMPILE_RESOURCES
# **********************
AC_ARG_VAR([GLIB_COMPILE_RESOURCES],[glib-compile-resources bin])
AC_PATH_PROG([GLIB_COMPILE_RESOURCES],[glib-compile-resources],[])
if test -z "$GLIB_COMPILE_RESOURCES"; then
  AC_MSG_ERROR([glib-compile-resources not found])
fi

# *******
# XMLLINT
# *******
AC_ARG_VAR([XMLLINT],[xmllint bin])
AC_PATH_PROG([XMLLINT],[xmllint],[])
if test -z "$XMLLINT"; then
  AC_MSG_ERROR([xmllint not found])
fi

dnl ###########################################################################
dnl Documentation
dnl ###########################################################################

YELP_HELP_INIT

dnl ###########################################################################
dnl Files to generate
dnl ###########################################################################

AC_CONFIG_FILES([
Makefile
src/Makefile
po/Makefile.in
data/Makefile
help/Makefile
])
AC_OUTPUT

echo "
Configure summary:

    ${PACKAGE_STRING}
    `echo $PACKAGE_STRING | sed "s/./=/g"`

    prefix:                       ${prefix}
    source code location:         ${srcdir}
    compiler:                     ${CC}
    compiler flags:               ${CFLAGS}
    warning flags:                ${WARN_CFLAGS}
"
