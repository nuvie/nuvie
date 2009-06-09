SUBDIRS = tools

bin_PROGRAMS = nuvie

CXXFLAGS = @CXXFLAGS@ -Iactors -IGUI -Imisc -Ifiles -Ifonts -Iconf -Ipathfinder -Isave -Iscreen -Iscript -Isound -Isound/adplug -Itools -Iusecode -Iviews -Ilua
LDFLAGS = @LIBS@

# Makefile.common defines nuvie_SOURCES
include Makefile.common

nuviedatadir = $(datadir)/nuvie
dist_nuviedata_DATA = \
	data/BorderU6_1.bmp \
	data/BorderU6_2.bmp \
	data/BorderU6_3.bmp \
	data/BorderU6_4.bmp \
	data/BorderU6_5.bmp \
	data/BorderU6_6.bmp \
	data/BorderU6_7.bmp \
	data/BorderU6_8.bmp \
	data/ScrollBarDown_1.bmp \
	data/ScrollBarDown_2.bmp \
	data/ScrollBarUp_1.bmp \
	data/ScrollBarUp_2.bmp

desktopdir=$(datadir)/applications
dist_desktop_DATA = builds/linux/nuvie.desktop

icondir=$(datadir)/icons
dist_icon_DATA = builds/linux/nuvie.png

EXTRA_DIST=nuvie.cfg.sample builds/linux/nuvie.cfg.in \
	builds/linux/nuvie.spec.in nuvie.spec
