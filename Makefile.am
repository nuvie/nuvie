SUBDIRS = tools

bin_PROGRAMS = nuvie

CXXFLAGS = @CXXFLAGS@ -Iactors -IGUI -Imisc -Ifiles -Ifonts -Iconf -Ipathfinder -Isave -Iscreen -Iscript -Isound -Isound/adplug -Isound/mixer -Itools -Iusecode -Iviews -Ilua -Ikeybinding
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
	data/ScrollBarUp_2.bmp \
	data/defaultkeys.txt

desktopdir=$(datadir)/applications
dist_desktop_DATA = builds/linux/nuvie.desktop

icondir=$(datadir)/icons
dist_icon_DATA = builds/linux/nuvie.png

EXTRA_DIST=nuvie.cfg.sample builds/linux/nuvie.cfg.in \
	builds/linux/nuvie.spec.in nuvie.spec


if MACOSX

bundle_name = Nuvie.app

bundle: ${EXE_TARGET}
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources/data/
	mkdir -p $(bundle_name)/Contents/Resources/English.lproj/
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	$(INSTALL_DATA) xcodebuild/Info-Nuvie.plist $(bundle_name)/Contents/Info.plist
	$(INSTALL_PROGRAM) nuvie$(EXEEXT) $(bundle_name)/Contents/MacOS/
	$(INSTALL_DATA) xcodebuild/nuvie.icns $(bundle_name)/Contents/Resources/
	$(INSTALL_DATA) xcodebuild/English.lproj/InfoPlist.strings $(bundle_name)/Contents/Resources/English.lproj/
	cp -r data/* $(bundle_name)/Contents/Resources/data
if WITH_OSX_CODE_SIGNATURE
	codesign -f -s "$(OSX_CODE_SIGNATURE)" $(bundle_name)
endif
	
install-exec-local: bundle
	mkdir -p $(DESTDIR)/Applications/
	cp -r $(bundle_name) $(DESTDIR)/Applications/

osxdmg: bundle
	mkdir Nuvie-snapshot
	cp $(srcdir)/AUTHORS ./Nuvie-snapshot/Authors
	cp $(srcdir)/COPYING ./Nuvie-snapshot/License
	cp $(srcdir)/changelog ./Nuvie-snapshot/Changelog
	cp $(srcdir)/README ./Nuvie-snapshot/ReadMe
	cp $(srcdir)/xcodebuild/misc/README_MacOSX.txt ./Nuvie-snapshot/ReadMe.MacOSX
	SetFile -t ttro -c ttxt ./Nuvie-snapshot/Authors
	SetFile -t ttro -c ttxt ./Nuvie-snapshot/License
	SetFile -t ttro -c ttxt ./Nuvie-snapshot/Changelog
	SetFile -t ttro -c ttxt ./Nuvie-snapshot/ReadMe
	SetFile -t ttro -c ttxt ./Nuvie-snapshot/ReadMe.MacOSX
	CpMac -r $(bundle_name) ./Nuvie-snapshot/
	hdiutil create -ov -format UDZO -imagekey zlib-level=9 -fs HFS+ \
					-srcfolder Nuvie-snapshot \
					-volname "Nuvie SVN snapshot" \
					Nuvie-snapshot.dmg
	rm -rf Nuvie-snapshot

clean-local:
	-rm -f Nuvie-snapshot.dmg
	-rm -rf $(bundle_name)

else

install-exec-local:

clean-local:
	
endif
