AC_INIT(src/main.cpp)

AM_CONFIG_HEADER(config.h)

dnl package name and version number

PACKAGE=monster-masher
VERSION=1.8

AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

dnl checks for programs
AC_PROG_INTLTOOL
AC_PROG_CXX

dnl checks for libraries
PKG_CHECK_MODULES(DEPS, libgnome-2.0 >= 2.0.0 \
                        gconfmm-2.6 >= 2.6.0 \
                        gtkmm-2.4 >= 2.6.0 \
                        libgnomecanvasmm-2.6 >= 2.6.0 \
			libglademm-2.4 >= 2.4.0)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

	
dnl GConf
AC_PATH_PROG(GCONFTOOL, gconftool-2, no)

if test x"$GCONFTOOL" = xno; then
AC_MSG_ERROR([gconftool-2 executable not found in your path - should be installed with GConf])
fi

AM_GCONF_SOURCE_2


dnl i18n
ALL_LINGUAS="az bg ca cs cy da de el en_CA en_GB es eu fi fr ga gu hr it ka lt ne nl no pa pl pt pt_BR rw sq sr sr@Latn sv ta tr uk zh_CN zh_TW"
	
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", "definition of GETTEXT_PACKAGE")


dnl extra stuff

AM_MAINTAINER_MODE
AC_C_BIGENDIAN

dnl create makefiles
AC_OUTPUT(
	Makefile
	src/Makefile
	pixmaps/Makefile
	sounds/Makefile
	po/Makefile.in
dnl	help/Makefile
dnl	help/C/Makefile
)
