#!/usr/bin/perl

#Frequently Changed Settings

#Correct include directories for freetype, zlib, libpng, XPM, JPEG, etc. 
#If your include directories are different, add them here. *Extra*
#directories that don't happen to exist on your system shouldn't
#cause an error (if they do, let me know).

$includeDirs = "-I/usr/include/freetype2 -I/usr/local/include/freetype2 " .
	"-I/usr/include/X11 -I/usr/X11R6/include/X11 -I/usr/local/include";

#Typical install locations for freetype, zlib, Xpm and libpng libraries.
#If yours are somewhere else, other than a standard location
#such as /lib or /usr/lib, then change this. This line shouldn't hurt 
#if you don't actually have some of the optional libraries and directories.
$libDirs = "-L/usr/lib/X11 -L/usr/X11R6/lib -L/usr/local/lib";

#If you don't have gcc, get it. If you really must, you can change this; 
#but if you do, you must specify not only your compiler but options 
#appropriate for shared library object creation. Just get gcc.
$compiler = "gcc -fPIC";

#Default installation prefix if --prefix is not used.
$installPrefix = "/usr";

#End Of Frequently Changed Settings
#
#(You may, however, need to work on the shared link command
#detector below if your operating system is not successfully
#accommodated; surprisingly, I am not perfect)

for ($i = 0; ($i < int(@ARGV)); $i++) {
	if ($ARGV[$i] eq "--prefix") {
		if ($ARGV[$i + 1] eq "") {
			die "No directory specified for --prefix\n";
		} else {
			$installPrefix = $ARGV[$i + 1];
			$i++;
		}
	} else {
		die "Usage: ./configure [--prefix /usr|/usr/local|et cetera]\n";
	}
}

print "gd configuration program\n";
print "TBB 10/21/02\n\n";

$os = `uname`;

if (!&testCompiler) {
	die "C compiler not found! Change the \$compiler setting at the top of the\n'configure' script and run configure again.\n";
}

#If we don't know, we try the elegant Linux way

if ($os =~ /^(linux|irix|tru64|ultrix|openbsd|netbsd|freebsd)/i) {
	$sharedLinkHead = "ld -shared";
	$sharedLinkTail = "";
	print "Found OS with linux-like shared library link command\n";
} elsif ($os =~ /^(sunos)/i) {
	$sharedLinkHead = "/usr/ccs/bin/ld -G";
	$sharedLinkTail = "-ldl";
	$socketLibForXpm = 1;
	print "Found OS with sunos-like shared library link command\n";
} elsif ($os =~ /^(darwin)/i) {
	$sharedLinkHead = "ld -dynamic -flat_namespace -undefined suppress";
	$sharedLinkTail = "";
	print "Found MacOS X, using appropriate shared library link command\n";
} else {
	$sharedLinkHead = "ld -shared";
	$sharedLinkTail = "";
	print "Unknown OS $os, trying linux-like shared library link command\n";
}

print "Shared library link command: $sharedLinkHead $sharedLinkTail\n";
	
if (&testLibrary("png", "png_create_read_struct (0, 0, 0, 0)", "-lz")) {
	push @options, "png";
	print "png library found.\n";
} else {
	print "Warning: png library not found, png will not be supported.\n";
}

if (&testLibrary("z", "deflate (0, 0)")) {
	push @options, "z";
	print "zlib library found.\n";
	$zfound = 1;
} else {
	print "Warning: zlib library not found, png (which you might want a lot) and\n",
		"gd2 (which you probably don't need) will not be supported.\n"; 
}

if (&testLibrary("jpeg", "jpeg_set_defaults (0)")) {
	push @options, "jpeg";
	print "jpeg library found.\n";
} else {
	print "Warning: jpeg library not found, jpeg will not be supported.\n";
}

if (&testLibrary("freetype", "FT_Init_FreeType(0)")) {
	push @options, "freetype";
	print "freetype 2.x library found.\n";
} else {
	print "Warning: freetype 2.x library not found, freetype will not be supported.\n";
}

#2.0.4 thanks to Len Makin: need optional libraries and uppercase X,
#also -lsocket under Solaris

if ($socketLibNeededForXpm) {
	$xpmLibs = "-lX11 -lsocket";
} else {
	$xpmLibs = "-lX11";
}

if (&testLibrary("Xpm", "XpmReadFileToXpmImage(0, 0, 0)", $xpmLibs)) {
	push @options, "Xpm";
	push @options, "X11";
	if ($socketLibNeededForXpm) {
		push @options, "socket";
	}
	print "Xpm library found.\n";
} else {
	print "Xpm library not found. That's OK. Almost no one needs Xpm in gd.\n";
}

for $o (@options) {
	$options{$o} = 1;
}

if (!int(@options)) {
	print <<EOM
*******************************************************************

WARNING: NONE of the libraries needed to produce popular image
formats were found. This is not a good thing. The library can
be compiled, but it will not be able to produce PNG or JPEG
or XPM images. Only a few minor formats can be supported without
libraries. "make test" will not succeed without libraries; this
is to be expected. IF YOU ARE NOT SURE THIS IS OK, you should go 
get libpng, libjpeg and libz now, and install them. Then run
configure again.

*******************************************************************
EOM
;
}

print "Optional libraries found: @options\n";

for $o (@options) {
	$oflags .= " -DHAVE_LIB" . uc($o);
	$lflags .= " -l$o";
}

if ($options{"png"}) {
	$safePrograms = "pngtogd pngtogd2 gdtopng gd2topng gd2copypal gdparttopng webpng";
} 
 
if ($options{"freetype"} && $options{"jpeg"}) {
	$safePrograms .= " annotate";
} 

@programs = split(/ /, $safePrograms);
for $p (@programs) {
	$installCommands .= "\tsh ./install-item 755 $p \$(INSTALL_BIN)/$p\n";
}

open(OUT, ">Makefile");
print OUT <<EOM

#Command for building a shared library. This varies depending on the OS.

LINK_SHARED_HEAD=$sharedLinkHead
LINK_SHARED_TAIL=$sharedLinkTail

#If the ar command fails on your system, consult the ar manpage
#for your system. 
AR=ar

CFLAGS=-g $oflags

LIBS=-lgd $lflags -lm

INCLUDEDIRS=-I. $includeDirs

LIBDIRS=$libDirs

INSTALL_LIB=$installPrefix/lib

INSTALL_INCLUDE=$installPrefix/include

INSTALL_BIN=$installPrefix/bin

#
#
# Changes should not be required below here.
#
#

# Update these with each release!

MAJOR_VERSION=2
VERSION=2.0.4

COMPILER=$compiler

CC=\$(COMPILER) \$(INCLUDEDIRS)
LINK=\$(CC) \$(LIBDIRS) \$(LIBS)

PROGRAMS=$safePrograms \$(TEST_PROGRAMS)

TEST_PROGRAMS=gdtest gddemo gd2time gdtestft testac fontwheeltest fontsizetest

default: instructions

instructions:
	\@echo Edit this Makefile if you wish. The configure script makes
	\@echo reasonable guesses, but as of this writing \\(2.0.2\\) has not
	\@echo been field-tested on a lot of systems.
	\@echo
	\@echo Second, type \\'make install\\' as root.
	\@echo
	\@echo This installs the GD \${VERSION} shared library,
	\@echo which is required in order to use the included
	\@echo utility programs, and also includes the utility
	\@echo programs such as webpng, pngtogd, etc.
	\@echo 
	\@echo OPTIONAL third step: type \\'make test\\' to build 
	\@echo the optional test programs. Type \\'make install\\' FIRST.
	\@echo
	\@echo IF SHARED LIBRARIES WON\\'T WORK FOR YOU: 
	\@echo Type \\'make libgd.a\\' to produce a static library,
	\@echo which you can install manually in /usr/lib.

test: \$(TEST_PROGRAMS)

install: libgd.so.\${VERSION} $safePrograms
$installCommands	sh ./install-item 755 bdftogd \$(INSTALL_BIN)/bdftogd
	sh ./install-item 644 gd.h \$(INSTALL_INCLUDE)/gd.h
	sh ./install-item 644 gdcache.h \$(INSTALL_INCLUDE)/gdcache.h
	sh ./install-item 644 gd_io.h \$(INSTALL_INCLUDE)/gd_io.h
	sh ./install-item 644 gdfontg.h \$(INSTALL_INCLUDE)/gdfontg.h
	sh ./install-item 644 gdfontl.h \$(INSTALL_INCLUDE)/gdfontl.h
	sh ./install-item 644 gdfontmb.h \$(INSTALL_INCLUDE)/gdfontmb.h
	sh ./install-item 644 gdfonts.h \$(INSTALL_INCLUDE)/gdfonts.h
	sh ./install-item 644 gdfontt.h \$(INSTALL_INCLUDE)/gdfontt.h

gddemo: gddemo.o
	\$(CC) gddemo.o -o gddemo	\$(LIBDIRS) \$(LIBS)

testac: testac.o
	\$(CC) testac.o -o testac	\$(LIBDIRS) \$(LIBS)

pngtogd: pngtogd.o
	\$(CC) pngtogd.o -o pngtogd	\$(LIBDIRS) \$(LIBS) 

webpng: webpng.o
	\$(CC) webpng.o -o webpng	\$(LIBDIRS) \$(LIBS)

annotate: annotate.o
	\$(CC) annotate.o -o annotate	\$(LIBDIRS) \$(LIBS)

pngtogd2: pngtogd2.o
	\$(CC) pngtogd2.o -o pngtogd2	\$(LIBDIRS) \$(LIBS)

gdtopng: gdtopng.o
	\$(CC) gdtopng.o -o gdtopng	\$(LIBDIRS) \$(LIBS)

gd2topng: gd2topng.o
	\$(CC) gd2topng.o -o gd2topng	\$(LIBDIRS) \$(LIBS)

gd2copypal: gd2copypal.o
	\$(CC) gd2copypal.o -o gd2copypal	\$(LIBDIRS) \$(LIBS)

gdparttopng: gdparttopng.o
	\$(CC) gdparttopng.o -o gdparttopng	\$(LIBDIRS) \$(LIBS)

gdtest: gdtest.o
	\$(CC) gdtest.o -o gdtest	\$(LIBDIRS) \$(LIBS)

gd2time: gd2time.o
	\$(CC) gd2time.o -o gd2time	\$(LIBDIRS) \$(LIBS)

gdtestft: gdtestft.o
	\$(CC) --verbose gdtestft.o -o gdtestft \$(LIBDIRS) \$(LIBS)

fontwheeltest: fontwheeltest.o
	\$(CC) --verbose fontwheeltest.o -o fontwheeltest \$(LIBDIRS) \$(LIBS)

fontsizetest: fontsizetest.o
	\$(CC) --verbose fontsizetest.o -o fontsizetest \$(LIBDIRS) \$(LIBS)

LIBOBJS=gd.o gd_gd.o gd_gd2.o gd_io.o gd_io_dp.o \\
		gd_io_file.o gd_ss.o gd_io_ss.o gd_png.o gd_jpeg.o gdxpm.o \\
		gdfontt.o gdfonts.o gdfontmb.o gdfontl.o gdfontg.o \\
		gdtables.o gdft.o gdcache.o gdkanji.o wbmp.o \\
		gd_wbmp.o gdhelpers.o gd_topal.o 

#Shared library. This should work fine on any ELF platform (Linux, etc.) with
#GNU ld or something similarly intelligent. To avoid the chicken-and-egg
#problem, this target also installs the library so that applications can
#actually find it.

libgd.so.\${VERSION}: \${LIBOBJS}
	-rm -f libgd.so.\${VERSION} 2>/dev/null
	\${LINK_SHARED_HEAD} -o libgd.so.\${VERSION} \${LIBOBJS} ${LINK_SHARED_TAIL}
	sh ./install-item 644 libgd.so.\${VERSION} \\
		\$(INSTALL_LIB)/libgd.so.\${VERSION}
	-rm \$(INSTALL_LIB)/libgd.so.\${MAJOR_VERSION} 2>/dev/null
	ln -s \$(INSTALL_LIB)/libgd.so.\${VERSION} \\
		\$(INSTALL_LIB)/libgd.so.\${MAJOR_VERSION}	
	-rm \$(INSTALL_LIB)/libgd.so 2>/dev/null
	ln -s \$(INSTALL_LIB)/libgd.so.\${VERSION} \\
		\$(INSTALL_LIB)/libgd.so	

#Static library, if you really need one for some reason.
libgd.a: \${LIBOBJS}
	rm -f libgd.a
	\$(AR) rc libgd.a \${LIBOBJS}
	-ranlib libgd.a

clean:
	rm -f *.o *.a *.so *.so.* \${PROGRAMS} test/gdtest.jpg test/gdtest.wbmp test/fttest.png test/fttest.jpg *test.errors font*test?.png
veryclean: clean
	rm Makefile
EOM
;

close(OUT);
print "\nMakefile created! Type 'make install' to build and install the\n";
print "gd library. You may wish to edit the Makefile first if you are\n";
print "not pleased with the results of library detection.\n\n";
print "The installation prefix is: $installPrefix\n\n";
print "If this doesn't appeal to you, run configure again with the \n";
print "--prefix option.\n\n";
print "If you have a rough time building the shared library, you can type\n";
print "'make libgd.a' to build a static library.\n\n";

sub testLibrary
{
	my($library, $function, $reqLibraries) = @_;
	my($ltest) = "gd-libtest";
	system("rm -rf $ltest");
	if (!mkdir("$ltest", 0700)) {
		die "Can't create subdirectory \"$ltest\" to test libraries.\n";
	}
	open(OUT, ">$ltest/libtest.c");
	print OUT <<EOM
int main(int argc, char *argv[])
{
	$function;
	return 0;
}
EOM
;
	# 2.03: have to close!
	close(OUT);
	# 2.03: make sure we pass the math library, many
	# platforms require it separately
	# 2.03: >& is a bash-ism, can't rely on it
	my($result) = system("cd $ltest; $compiler libtest.c -o libtest $libDirs -l$library -lm $reqLibraries > ../libtest.errors 2>&1");
	system("rm -rf $ltest");
	if ($result != 0) {
		return 0;
	} else {
		return 1;
	}
}

sub testCompiler
{
	my($ctest) = "gd-comptest";
	system("rm -rf $ctest");
	if (!mkdir("$ctest", 0700)) {
		die "Can't create subdirectory \"$ctest\" to test compiler.\n";
	}
	open(OUT, ">$ctest/compilertest.c");
	print OUT <<EOM
int main(int argc, char *argv[])
{
	return 0;
}
EOM
;
	# 2.03: have to close!
	close(OUT);
	# 2.03: correct for all sh, not just bash
	my($result) = system("cd $ctest; $compiler compilertest.c -o compilertest > ../compilertest.errors 2>&1");
	system("rm -rf $ctest");
	if ($result != 0) {
		return 0;
	} else {
		return 1;
	}
}
	
