# tools

MAKE=make
PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker
XCODEBUILD=xcodebuild

# local

PREBUILD=$(CURDIR)/../prebuild
LOCAL=$(PREBUILD)/usr/local
LOCAL_BIN=$(LOCAL)/bin
LOCAL_INCLUDE=$(LOCAL)/include
LOCAL_LIB=$(LOCAL)/lib

# base

GV_DIR=../..
DOT=dot

#
# graphviz
#

graphviz.pkg: graphviz.pmdoc usr/local/bin/$(DOT) Release/Graphviz.app postinstall
	@echo
	@echo PACKAGING GRAPHVIZ...
	@echo
	"$(PACKAGEMAKER)" --doc graphviz.pmdoc --out $@

usr/local/bin/$(DOT): $(GV_DIR)/cmd/dot/.libs/$(DOT)
	@echo
	@echo INSTALLING GRAPHVIZ...
	@echo
	$(MAKE) DESTDIR=$(CURDIR) -C $(GV_DIR) install-strip
	rm -rf usr/local/lib/graphviz/tcl/libtcldot_builtin.*
	
Release/Graphviz.app: ../*.m ../*.h $(GV_DIR)/cmd/dot/.libs/$(DOT)
	@echo
	@echo BUILDING GRAPHVIZ GUI...
	@echo
	"$(XCODEBUILD)" -project ../graphviz.xcodeproj -configuration Release

postinstall:
	echo "/usr/local/bin/dot -c" >$@
	make -n -C ../../tclpkg install-data-hook >>$@

$(GV_DIR)/cmd/dot/.libs/$(DOT): $(GV_DIR)/Makefile
	@echo
	@echo MAKING GRAPHVIZ...
	@echo
	if [[ -e $@ ]]; then $(MAKE) -C $(GV_DIR) clean; fi
	$(MAKE) -C $(GV_DIR)/tclpkg/tkstubs
	$(MAKE) -C $(GV_DIR)/tclpkg/tclstubs
	$(MAKE) -C $(GV_DIR)/tclpkg/tkspline CFLAGS="-O2 -arch ppc -arch i386 -I$(LOCAL_INCLUDE)" LDFLAGS="-arch ppc -arch i386 -L$(LOCAL_LIB) -Wl,-dead_strip -Wl,-dead_strip_dylibs"
	$(MAKE) -C $(GV_DIR)
	
$(GV_DIR)/Makefile: $(GV_DIR)/configure
	@echo
	@echo CONFIGURING GRAPHVIZ...
	@echo
	cd $(GV_DIR) && ./configure --disable-dependency-tracking --with-ipsepcola --with-quartz CFLAGS="-O2 -arch ppc -arch i386 -arch ppc64 -I$(LOCAL_INCLUDE)" CXXFLAGS="-O2 -arch ppc -arch i386 -arch ppc64 -I$(LOCAL_INCLUDE)" LDFLAGS="-arch ppc -arch i386 -arch ppc64 -L$(LOCAL_LIB) -Wl,-dead_strip -Wl,-dead_strip_dylibs" GDLIB_CONFIG="$(LOCAL_BIN)/gdlib-config" PKG_CONFIG="$(LOCAL_BIN)/pkg-config" PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig:$(LOCAL_LIB)/pkgconfig"
