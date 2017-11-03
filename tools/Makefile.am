if BUILD_TOOLS
noinst_PROGRAMS = u6decomp unpack_conv unpack_portraits unpack_font unpack_maptiles unpack_talk unpack_lzc pack_font pack_lzc
else
noinst_PROGRAMS =
endif

CXXFLAGS = @CXXFLAGS@ -I ../misc -I ../conf
CFLAGS = @CXXFLAGS@ 

# Makefile.common defines u6decomp_SOURCES
include Makefile.common
