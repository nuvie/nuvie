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
AC_ARG_ENABLE(debug,
  [  --enable-debug          enable debugging (disables optimizations)],
  [ DEBUG_FLAGS="-O -g -Wall"
    AC_MSG_RESULT(yes)
  ], AC_MSG_RESULT(no)
)

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

CXXFLAGS="$CXXFLAGS $DEBUG_FLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CONFIG_FILES([
Makefile
tools/Makefile
])
AC_OUTPUT
