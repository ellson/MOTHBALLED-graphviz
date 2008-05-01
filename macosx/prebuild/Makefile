# tools

CURL=curl
MAKE=make
TAR=tar

# local

LOCAL=/usr/local
LOCAL_LIB=$(LOCAL)/lib
LOCAL_BIN=$(LOCAL)/bin

# package URL

PKGCONFIG_URL=http://pkgconfig.freedesktop.org/releases/pkg-config-0.23.tar.gz
GD_URL=http://www.libgd.org/releases/gd-2.0.35.tar.gz
PIXMAN_URL=http://cairographics.org/releases/pixman-0.10.0.tar.gz
CAIRO_URL=http://cairographics.org/releases/cairo-1.6.4.tar.gz
GETTEXT_URL=ftp://mirrors.kernel.org/gnu/gettext/gettext-0.17.tar.gz
GLIB_URL=http://ftp.gnome.org/pub/GNOME/sources/glib/2.16/glib-2.16.3.tar.gz
PANGO_URL=http://ftp.gnome.org/pub/GNOME/sources/pango/1.20/pango-1.20.2.tar.gz

# libraries

PKGCONFIG=pkg-config
GD=libgd.la
PIXMAN=libpixman-1.la
CAIRO=libcairo.la
INTL=libintl.la
GLIB=libglib-2.0.la
PANGO=libpango-1.0.la

# directories

PKGCONFIG_DIR=$(basename $(basename $(notdir $(PKGCONFIG_URL))))
GD_DIR=$(basename $(basename $(notdir $(GD_URL))))
PIXMAN_DIR=$(basename $(basename $(notdir $(PIXMAN_URL))))
CAIRO_DIR=$(basename $(basename $(notdir $(CAIRO_URL))))
GETTEXT_DIR=$(basename $(basename $(notdir $(GETTEXT_URL))))
GLIB_DIR=$(basename $(basename $(notdir $(GLIB_URL))))
PANGO_DIR=$(basename $(basename $(notdir $(PANGO_URL))))

#
# target
#

install: $(LOCAL_LIB)/$(PANGO) $(LOCAL_LIB)/$(GD)

uninstall:
	sudo $(MAKE) -C $(PANGO_DIR) uninstall
	sudo $(MAKE) -C $(GLIB_DIR) uninstall
	sudo $(MAKE) -C $(GETTEXT_DIR) uninstall
	sudo $(MAKE) -C $(CAIRO_DIR) uninstall
	sudo $(MAKE) -C $(PIXMAN_DIR) uninstall
	sudo $(MAKE) -C $(GD_DIR) uninstall
	sudo $(MAKE) -C $(PKGCONFIG_DIR) uninstall
	
clean:
	sudo $(MAKE) -C $(PANGO_DIR) clean
	sudo $(MAKE) -C $(GLIB_DIR) clean
	sudo $(MAKE) -C $(GETTEXT_DIR) clean
	sudo $(MAKE) -C $(CAIRO_DIR) clean
	sudo $(MAKE) -C $(PIXMAN_DIR) clean
	sudo $(MAKE) -C $(GD_DIR) clean
	sudo $(MAKE) -C $(PKGCONFIG_DIR) clean
	
distclean:
	sudo $(MAKE) -C $(PANGO_DIR) distclean
	sudo $(MAKE) -C $(GLIB_DIR) distclean
	sudo $(MAKE) -C $(GETTEXT_DIR) distclean
	sudo $(MAKE) -C $(CAIRO_DIR) distclean
	sudo $(MAKE) -C $(PIXMAN_DIR) distclean
	sudo $(MAKE) -C $(GD_DIR) distclean
	sudo $(MAKE) -C $(PKGCONFIG_DIR) distclean

#
# pango
#

$(LOCAL_LIB)/$(PANGO): $(PANGO_DIR)/pango/$(PANGO)
	@echo
	@echo INSTALLING PANGO...
	@echo
	sudo $(MAKE) -C $(PANGO_DIR) install

$(PANGO_DIR)/pango/$(PANGO): $(PANGO_DIR)/Makefile
	@echo
	@echo MAKING PANGO...
	@echo
	if [[ -e $@ ]]; then $(MAKE) -C $(PANGO_DIR) clean; fi && \
	$(MAKE) -C $(PANGO_DIR)
	
$(PANGO_DIR)/Makefile: $(LOCAL_LIB)/$(CAIRO) $(LOCAL_LIB)/$(GLIB) | $(LOCAL_BIN)/$(PKGCONFIG) $(PANGO_DIR)/configure
	@echo
	@echo CONFIGURING PANGO...
	@echo
	cd $(PANGO_DIR) && ./configure --disable-dependency-tracking --enable-static=yes --enable-shared=no --with-included_modules=yes CFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64" LDFLAGS="-arch ppc -arch i386 -arch ppc64" LIBS="/System/Library/Frameworks/CoreFoundation.framework/CoreFoundation /System/Library/Frameworks/CoreServices.framework/CoreServices /System/Library/Frameworks/ApplicationServices.framework/ApplicationServices" PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig"

$(PANGO_DIR)/configure: | $(PANGO_DIR).tar.gz
	@echo
	@echo UNTARRING PANGO...
	@echo
	$(TAR) xzf $|
		
$(PANGO_DIR).tar.gz:
	@echo
	@echo TRANSFERRING PANGO...
	@echo
	$(CURL) --remote-name --location $(PANGO_URL) || rm -f $(PANGO_DIR).tar.gz

#
# glib
#

$(LOCAL_LIB)/$(GLIB): $(GLIB_DIR)/glib/$(GLIB)
	@echo
	@echo INSTALLING GLIB...
	@echo
	sudo $(MAKE) -C $(GLIB_DIR) install
	
$(GLIB_DIR)/glib/$(GLIB): $(GLIB_DIR)/Makefile
	@echo
	@echo MAKING GLIB...
	@echo
	if [[ -e $@ ]]; then $(MAKE) -C $(GLIB_DIR) clean; fi && \
	$(MAKE) -C $(GLIB_DIR)
	
$(GLIB_DIR)/Makefile: $(LOCAL_LIB)/$(INTL) | $(LOCAL_BIN)/$(PKGCONFIG) $(GLIB_DIR)/configure 
	@echo
	@echo CONFIGURING GLIB...
	@echo
	cd $(GLIB_DIR) && \
	OSREL=`uname -r` && \
	BUILD=`./config.guess` && \
	rm -f darwin.cache && \
	echo "glib_cv_long_long_format=ll" >>darwin.cache && \
	echo "glib_cv_stack_grows=no" >>darwin.cache && \
	echo "glib_cv_uscore=no" >>darwin.cache && \
	echo "ac_cv_func_posix_getpwuid_r=yes" >>darwin.cache && \
	echo "ac_cv_func_posix_getgrgid_r=yes" >>darwin.cache && \
	echo "ac_cv_func_vsnprintf_c99=yes" >>darwin.cache && \
	echo "ac_cv_func_printf_unix98=yes" >>darwin.cache && \
	echo --- GLIB-GENMARSHAL PASS --- && \
	./configure --disable-dependency-tracking --enable-static=yes --enable-shared=no PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig" && \
	$(MAKE) -C glib && \
	$(MAKE) -C gobject glib-genmarshal && \
	PATH=gobject:$$PATH && \
	cp darwin.cache darwinian.cache && \
	echo --- PPC PASS --- && \
	./configure --cache-file=darwinian.cache --build=$$BUILD --host=powerpc-apple-darwin$$OSREL --disable-dependency-tracking --enable-static=yes --enable-shared=no CFLAGS="-arch ppc" LDFLAGS="-arch ppc" PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig" && \
	cp config.h config.ppc.h && \
	cp glibconfig.h glibconfig.ppc.h && \
	cp darwin.cache darwinian.cache && \
	echo --- I386 PASS --- && \
	./configure --cache-file=darwinian.cache --build=$$BUILD --host=i686-apple-darwin$$OSREL --disable-dependency-tracking --enable-static=yes --enable-shared=no CFLAGS="-arch i386" LDFLAGS="-arch i386" PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig" && \
	cp config.h config.i386.h && \
	cp glibconfig.h glibconfig.i386.h && \
	cp darwin.cache darwinian.cache && \
	echo --- PPC64 PASS --- && \
	./configure --cache-file=darwinian.cache --build=$$BUILD --host=powerpc-apple-darwin$$OSREL --disable-dependency-tracking --enable-static=yes --enable-shared=no CFLAGS="-arch ppc64" LDFLAGS="-arch ppc64" PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig" && \
	cp config.h config.ppc64.h && \
	cp glibconfig.h glibconfig.ppc64.h && \
	cp darwin.cache darwinian.cache && \
	echo --- X86_64 PASS --- && \
	./configure --cache-file=darwinian.cache --build=$$BUILD --host=x86_64-apple-darwin$$OSREL --disable-dependency-tracking --enable-static=yes --enable-shared=no CFLAGS="-arch x86_64" LDFLAGS="-arch x86_64" PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig" && \
	cp config.h config.x86_64.h && \
	cp glibconfig.h glibconfig.x86_64.h && \
	echo --- UNIVERSAL BINARY PASS --- && \
	./configure --disable-dependency-tracking --enable-static=yes --enable-shared=no CFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64" LDFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64" PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig" && \
	rm config.h && \
	echo "#if defined(__ppc__)" >>config.h && \
	cat config.ppc.h >>config.h && \
	echo "#elif defined(__i386__)" >>config.h && \
	cat config.i386.h >>config.h && \
	echo "#elif defined(__ppc64__)" >>config.h && \
	cat config.ppc64.h >>config.h && \
	echo "#elif defined(__x86_64__)" >>config.h && \
	cat config.x86_64.h >>config.h && \
	echo "#endif" >>config.h && \
	rm glibconfig.h && \
	echo "#if defined(__ppc__)" >>glibconfig.h && \
	cat glibconfig.ppc.h >>glibconfig.h && \
	echo "#elif defined(__i386__)" >>glibconfig.h && \
	cat glibconfig.i386.h >>glibconfig.h && \
	echo "#elif defined(__ppc64__)" >>glibconfig.h && \
	cat glibconfig.ppc64.h >>glibconfig.h && \
	echo "#elif defined(__x86_64__)" >>glibconfig.h && \
	cat glibconfig.x86_64.h >>glibconfig.h && \
	echo "#endif" >>glibconfig.h && \
	touch stamp-gc-h && \
	$(MAKE) clean
	
$(GLIB_DIR)/configure: | $(GLIB_DIR).tar.gz
	@echo
	@echo CONFIGURING GLIB...
	@echo
	$(TAR) xzf $| && patch -p0 <glib.patch
	
$(GLIB_DIR).tar.gz:
	@echo
	@echo TRANSFERRING GLIB...
	@echo
	$(CURL) --remote-name --location $(GLIB_URL) || rm -f $(GLIB_DIR).tar.gz

#
# gettext
#

$(LOCAL_LIB)/$(INTL): $(GETTEXT_DIR)/gettext-runtime/intl/$(INTL)
	@echo
	@echo INSTALLING GETTEXT...
	@echo
	sudo $(MAKE) -C $(GETTEXT_DIR) install
	
$(GETTEXT_DIR)/gettext-runtime/intl/$(INTL): $(GETTEXT_DIR)/Makefile
	@echo
	@echo MAKING GETTEXT...
	@echo
	if [[ -e $@ ]]; then $(MAKE) -C $(GETTEXT_DIR) clean; fi && \
	$(MAKE) -C $(GETTEXT_DIR)
	
$(GETTEXT_DIR)/Makefile: | $(GETTEXT_DIR)/configure
	@echo
	@echo CONFIGURING GETTEXT...
	@echo
	cd $(GETTEXT_DIR) && ./configure --disable-dependency-tracking --enable-static=yes --enable-shared=no CFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64" LDFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64"
	
$(GETTEXT_DIR)/configure: | $(GETTEXT_DIR).tar.gz
	@echo
	@echo UNTARRING GETTEXT...
	@echo
	$(TAR) xzf $|
	
$(GETTEXT_DIR).tar.gz:
	@echo
	@echo TRANSFERRING GETTEXT...
	@echo
	$(CURL) --remote-name --location $(GETTEXT_URL) || rm -f $(GETTEXT_DIR).tar.gz

#
# cairo
#

$(LOCAL_LIB)/$(CAIRO): $(CAIRO_DIR)/src/$(CAIRO)
	@echo
	@echo INSTALLING CAIRO...
	@echo
	sudo $(MAKE) -C $(CAIRO_DIR) install
	
$(CAIRO_DIR)/src/$(CAIRO): $(CAIRO_DIR)/Makefile
	@echo
	@echo MAKING CAIRO...
	@echo
	if [[ -e $@ ]]; then $(MAKE) -C $(CAIRO_DIR) clean; fi && \
	$(MAKE) -C $(CAIRO_DIR)
	
$(CAIRO_DIR)/Makefile: $(LOCAL_LIB)/$(PIXMAN) | $(LOCAL_BIN)/$(PKGCONFIG) $(CAIRO_DIR)/configure
	@echo
	@echo CONFIGURING CAIRO...
	@echo
	cd $(CAIRO_DIR) && ./configure --disable-dependency-tracking --enable-static=yes --enable-shared=no CFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64" LDFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64" PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig"
	
$(CAIRO_DIR)/configure: | $(CAIRO_DIR).tar.gz
	@echo
	@echo UNTARRING CAIRO...
	@echo
	$(TAR) xzf $| && patch -p0 <cairo.patch
	
$(CAIRO_DIR).tar.gz:
	@echo
	@echo TRANSFERRING CAIRO...
	@echo
	$(CURL) --remote-name --location $(CAIRO_URL) || rm -f $(CAIRO_DIR).tar.gz

#
# pixman
#

$(LOCAL_LIB)/$(PIXMAN): $(PIXMAN_DIR)/pixman/$(PIXMAN)
	@echo
	@echo INSTALLING PIXMAN...
	@echo
	sudo $(MAKE) -C $(PIXMAN_DIR) install
	
$(PIXMAN_DIR)/pixman/$(PIXMAN): $(PIXMAN_DIR)/Makefile
	@echo
	@echo MAKING PIXMAN...
	@echo
	if [[ -e $@ ]]; then $(MAKE) -C $(PIXMAN_DIR) clean; fi && \
	$(MAKE) -C $(PIXMAN_DIR)
	
$(PIXMAN_DIR)/Makefile: | $(LOCAL_BIN)/$(PKGCONFIG) $(PIXMAN_DIR)/configure 
	@echo
	@echo CONFIGURING PIXMAN...
	@echo
	cd $(PIXMAN_DIR) && ./configure --disable-dependency-tracking --enable-static=yes --enable-shared=no CFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64" LDFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64" PKG_CONFIG_PATH="/usr/X11/lib/pkgconfig"
	
$(PIXMAN_DIR)/configure: | $(PIXMAN_DIR).tar.gz
	@echo
	@echo UNTARRING PIXMAN...
	@echo
	$(TAR) xzf $| && patch -p0 <pixman.patch
	
$(PIXMAN_DIR).tar.gz:
	@echo
	@echo TRANSFERRING PIXMAN...
	@echo
	$(CURL) --remote-name --location $(PIXMAN_URL) || rm -f $(PIXMAL_DIR).tar.gz

#
# gd
#

$(LOCAL_LIB)/$(GD): $(GD_DIR)/$(GD)
	@echo
	@echo INSTALLING GD...
	@echo
	sudo $(MAKE) -C $(GD_DIR) install
	
$(GD_DIR)/$(GD): $(GD_DIR)/Makefile
	@echo
	@echo MAKING GD...
	@echo
	if [[ -e $@ ]]; then $(MAKE) -C $(GD_DIR) clean; fi && \
	$(MAKE) -C $(GD_DIR)
	
$(GD_DIR)/Makefile: | $(GD_DIR)/configure
	@echo
	@echo CONFIGURING GD...
	@echo
	cd $(GD_DIR) && ./configure --disable-dependency-tracking --enable-static=yes --enable-shared=no CFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64" LDFLAGS="-arch ppc -arch i386 -arch ppc64 -arch x86_64"
	
$(GD_DIR)/configure: | $(GD_DIR).tar.gz
	@echo
	@echo UNTARRING GD...
	@echo
	$(TAR) xzf $|
	
$(GD_DIR).tar.gz:
	@echo
	@echo TRANSFERRING GD...
	@echo
	$(CURL) --remote-name --location $(GD_URL) || rm -f $(GD_DIR).tar.gz
	
#
# pkgconfig
#

$(LOCAL_BIN)/$(PKGCONFIG): $(PKGCONFIG_DIR)/$(PKGCONFIG)
	@echo
	@echo INSTALLING PKGCONFIG...
	@echo
	sudo $(MAKE) -C $(PKGCONFIG_DIR) install
	
$(PKGCONFIG_DIR)/$(PKGCONFIG): $(PKGCONFIG_DIR)/Makefile
	@echo
	@echo MAKING PKGCONFIG...
	@echo
	if [[ -e $@ ]]; then $(MAKE) -C $(PKGCONFIG_DIR) clean; fi && \
	$(MAKE) -C $(PKGCONFIG_DIR)
	
$(PKGCONFIG_DIR)/Makefile: | $(PKGCONFIG_DIR)/configure
	@echo
	@echo CONFIGURING PKGCONFIG...
	@echo
	cd $(PKGCONFIG_DIR) && ./configure --disable-dependency-tracking CFLAGS="-arch ppc -arch i386" LDFLAGS="-arch ppc -arch i386"
	
$(PKGCONFIG_DIR)/configure: | $(PKGCONFIG_DIR).tar.gz
	@echo
	@echo UNTARRING PKGCONFIG...
	@echo
	$(TAR) xzf $|
	
$(PKGCONFIG_DIR).tar.gz:
	@echo
	@echo TRANSFERRING PKGCONFIG...
	@echo
	$(CURL) --remote-name --location $(PKGCONFIG_URL) || rm -f $(PKGCONFIG_DIR).tar.gz

#
# phony
#

.PHONY: install uninstall clean distclean