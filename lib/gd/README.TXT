For documentation, browse index.html.

Quick install guide:

1. Type './configure'
2. Type 'make install'

Generic configuration instructions are in the file INSTALL.

The following 3rd-party libraries will be used by gd if found by configure.
While gd will compile and install even without these, we suggest that at
least zlib and libpng are installed, and recommend that freetype and jpeg
are installed as well:

1. zlib, available from http://www.gzip.org/zlib/
   Data compression library

2. libpng, available from http://www.libpng.org/pub/png/
   Portable Network Graphics library; requires zlib

3. FreeType 2.x, available from http://www.freetype.org/
   Free, high-quality, and portable font engine

4. JPEG library, available from http://www.ijg.org/
   Portable JPEG compression/decompression library

5. XPM, available from http://koala.ilog.fr/lehors/xpm.html
   X Pixmap library

If any of these libraries are installed, but not detected by configure,
you can use the following command line options:

--with-png=DIR

  Without the DIR argument, configure will check for png header files and
  libraries in a default location. To switch off png support, use
  --without-png, or --with-png=no.

  If the DIR argument is specified, configure tries to find the png header
  files in DIR/include, and the libraries in DIR/lib. To accommodate
  OpenBSD ports, DIR/include/libpng is also checked if necessary.

--with-freetype=DIR

  Dto. for freetype 2.x library. The search path for include files is
  DIR/include/freetype2.

--with-jpeg=DIR

  Dto. for jpeg library.

--with-xpm=DIR

  Dto. for xpm library.

Last but not least, if all else fails, you can still try Tom Boutell's
original, perl-based configure utility, which is included in this
distribution as configure.pl, or even the included makefile.sample.

