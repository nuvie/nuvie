AC_PREREQ(2.52)
AC_INIT(nuvie,0.1)
AC_CONFIG_SRCDIR(nuvie.cpp)

# check build/host/target systems
# (build  = system we're building on,
#  host   = system we're building for,
#  target = system the program we're building will build for)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AC_HEADER_STDC

case "$host_os" in
	linux*)
		CPPFLAGS="$CPPFLAGS -DLUA_USE_LINUX"
		;;
	darwin*)
		dnl We have a problem here: both MacOS X and Darwin report 
		dnl the same signature "powerpc-apple-darwin*" - so we have
		dnl to do more to distinguish them. Plain Darwin will propably
		dnl use X-Windows; and it is of course lacking Cocoa.
		dnl For now I am lazy and do not add proper detection code.
		AC_DEFINE(MACOSX, 1, [Mac OS X])
		AC_MSG_RESULT(Mac OS X)
		HOST_SYSTEM=MACOSX
		;;
	*)
		;;
esac

AM_CONDITIONAL(MACOSX, test x$HOST_SYSTEM = xMACOSX)

# SDL
AC_LANG_C
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
	:,
	AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
AC_LANG_CPLUSPLUS


# debugging
DEBUG_FLAGS="-O2"
AC_MSG_CHECKING(for debug options)
AC_ARG_ENABLE(debug, [  --disable-debug         disable debugging/warnings],,enable_debug=yes)
if test x$enable_debug = xyes; then
	DEBUG_FLAGS="-O -g -Wall"
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

# Build the tools?
AC_ARG_ENABLE(tools,    [  --disable-tools         Don't build the tools],,enable_tools=yes)
AC_MSG_CHECKING(whether to build the tools)
if test x$enable_tools = xno; then
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(BUILD_TOOLS, false)
else
        AC_MSG_RESULT(no)
        AM_CONDITIONAL(BUILD_TOOLS, true)
fi

# determining nuvie.cfg entries
#AC_ARG_WITH(u6, [  --with-u6=DIR          directory where U6 is installed ],U6PATH=$with_u6,U6PATH=${datadir}/games/u6/)
#AC_SUBST(U6PATH)

# ---------------------------------------------------------------------
# Black magic for static linking on OS X
# ---------------------------------------------------------------------
if test x$HOST_SYSTEM == xMACOSX; then
	AC_ARG_WITH(macosx-static-lib-path,
		AS_HELP_STRING([--with-macosx-static-lib-path=path], [path to location of static library files (Mac OS X bundles only)]),
		[with_macosx_static_lib_path="$withval"],
		[with_macosx_static_lib_path=""])
	if test x$with_macosx_static_lib_path != x; then
		if test ! -d $with_macosx_static_lib_path; then
			AC_MSG_ERROR([*** a directory is expected for --macosx-static-lib-path!])
		fi
		with_macosx_static_lib_path=`echo "$with_macosx_static_lib_path" | sed 's/\/*$//'`

		# Apple has done its best to prevent the above from working, so we
		# need this stuff here.
		# Using -static doesn't work because of system libraries that cannot be
		# linked statically; using the usual -Wl,-static -lvorbis -Wl,-dynamic
		# also fails if there is a non-static version of libvorbis in the lib
		# search path before or at the same location as the static lib because
		# the linker tries to use these even with the flags. The only way to get
		# reliable static compiles in Mac OS X is to hard-code the path to the
		# static library and link to *that*.
		# Damn you, Apple!

		# SDL MacOS X readme lists this as needed.
		# We should have sdl-config by this point, or we would have
		# died in PENT_CHECK_SDL.
		SDL_LIBS=`$SDL_CONFIG $sdl_config_args --static-libs`
	fi
fi

CXXFLAGS="$CXXFLAGS $DEBUG_FLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CONFIG_FILES([
Makefile
tools/Makefile
builds/linux/nuvie.cfg
nuvie.spec:builds/linux/nuvie.spec.in
])
AC_OUTPUT
