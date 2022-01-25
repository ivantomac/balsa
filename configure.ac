dnl Process this file with autoconf to produce a configure script.

# Initialisations
AC_INIT(bin/balsa-config.in)
AM_INIT_AUTOMAKE(balsa, 4.0)
AM_CONFIG_HEADER(config.h)

default_prefix=/usr/local/balsa

AC_PREFIX_DEFAULT($default_prefix)
if test "$prefix" = "NONE"
then
	prefix=$default_prefix
fi

dnl Update CFLAGS. -DPIC seems optional. I don't know why -fPIC is
dnl needed twice, but that's the only way it would work on Athlon64.
CFLAGS="-fPIC -fPIC -DPIC $CFLAGS"

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PATH_PROG(CG, cg)
AC_PATH_PROG(RPP, rpp)
AC_PATH_PROG(REX, rex)
AC_PATH_PROG(LALR, lalr)
AC_PATH_PROG(AST, ast)
AC_PATH_PROG(NM, nm)

AM_PROG_LIBTOOL

dnl Optional binaries / search path on failure to find
AC_PATH_PROG(GUILE, guile, guile)
AC_PATH_PROG(ESPRESSO, espresso, espresso)
AC_PATH_PROG(DOT, dot, dot)

balsadatadir=${prefix}/share

AC_SUBST(balsadatadir)

formatlibdir=${prefix}/lib
formatincdir=${prefix}/include/format
AC_SUBST(formatlibdir)
AC_SUBST(formatincdir)

formatguilibdir=${prefix}/lib
formatguiincdir=${prefix}/include/formatgui
AC_SUBST(formatguilibdir)
AC_SUBST(formatguiincdir)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(inttypes.h)

AC_CHECK_SIZEOF(void *)

dnl Extra arguments to handle the GMP library
AC_ARG_WITH(gmp_include,
   [  --with-gmp-include=DIR  GMP include directory ],
   CPPFLAGS="$CPPFLAGS -I$withval")
AC_ARG_WITH(gmp_lib,
   [  --with-gmp-lib=DIR      GMP lib directory ],
   LDFLAGS="$LDFLAGS -L$withval")

AC_CHECK_LIB(gmp,__gmpz_import)

if test ${ac_cv_lib_gmp___gmpz_import} = no ;
then
	echo Balsa needs version 4.1 or higher of the GMP library
	exit 1;
fi

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.0]);
PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.0]);
PKG_CHECK_MODULES([GTHREAD], [gthread-2.0 >= 2.0]);
PKG_CHECK_MODULES([GMODULE], [gmodule-2.0 >= 2.0]);

AC_DEFINE_UNQUOTED(BALSAHOME, "$prefix", "Compiled Balsa install directory")
AC_DEFINE_UNQUOTED(DEFAULT_BALSAPATH, "$balsadatadir:.", "Path to installed Balsa/Breeze files")

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_CHECK_FUNCS(uname)
AC_CHECK_FUNCS(fcntl)
AC_CHECK_FUNCS(fileno)
AC_CHECK_FUNCS(getpid)

AC_OUTPUT([Makefile
	doc/Makefile
	extra/Makefile
	extra/scintilla-wrapper/Makefile
	bin/Makefile
	bin/balsa-config
	bin/balsa-local-script
	bin/balsa-espresso
	bin/balsa-make-builtin-lib
	bin/balsa-scheme-script
	bin/balsa-scheme-shell
	src/Makefile
	src/libs/Makefile
	src/libs/format/Makefile
	src/libs/balsasim/Makefile
	src/libs/breeze/Makefile
	src/balsa-c/Makefile
	src/balsa-file-test/Makefile
	src/balsa-mgr/Makefile
	src/balsa-mgr/pixmaps/Makefile
	src/breeze-sim/Makefile
	src/breeze-sim/fast/Makefile
	src/breeze-sim/debug/Makefile
	src/breeze-sim/early/Makefile
	src/breeze-sim/mpi/Makefile
	src/breeze-sim-ctrl/Makefile
	src/breeze-sim-ctrl/pixmaps/Makefile
	share/Makefile
	share/scheme/Makefile
	share/scheme/base.scm
	share/scheme/base-local.scm
	share/scheme/globals.scm
	share/tech/Makefile
	share/tech/common/Makefile
	share/style/Makefile
	share/style/dual_b/Makefile
	share/style/four_b_rb/Makefile
	share/style/four_e_e/Makefile
	share/style/one_of_2_4/Makefile
	share/balsa/Makefile
	share/balsa/types/Makefile
	share/balsa/parts/Makefile
	share/balsa/sim/Makefile]
)
