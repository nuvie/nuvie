SUBDIRS = tools

bin_PROGRAMS = nuvie

CXXFLAGS = @CXXFLAGS@ -Iactors -IGUI -Imisc -Ifiles -Ifonts -Iconf -Ipathfinder -Isave -Iscreen -Isound -Isound/adplug -Itools -Iusecode -Iviews
LDFLAGS = @LIBS@

# Makefile.common defines nuvie_SOURCES
include Makefile.common


EXTRA_DIST=nuvie.cfg
