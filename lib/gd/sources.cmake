# $Id$ $Revision$

SET(gd_SRCDIR ${CMAKE_SOURCE_DIR}/lib/gd)

INCLUDE( UsePkgConfig )

INCLUDE( FindJPEG )
INCLUDE( FindPNG )
INCLUDE( FindZLIB )

PKGCONFIG( freetype2 FREETYPE_INCLUDE_DIR FREETYPE_LIB_DIR FREETYPE_LINK_FLAGS FREETYPE_CFLAGS )
PKGCONFIG( fontconfig FONTCONFIG_INCLUDE_DIR FONTCONFIG_LIB_DIR FONTCONFIG_LINK_FLAGS FONTCONFIG_CFLAGS )

INCLUDE_DIRECTORIES(
	${gd_SRCDIR}
	${FREETYPE_INCLUDE_DIR}
	${FONTCONFIG_INCLUDE_DIR}
)

ADD_DEFINITIONS(${FREETYPE_CFLAGS} ${FONTCONFIG_CFLAGS})

SET(gd_SRCS
	${gd_SRCDIR}/gdtables.c
	${gd_SRCDIR}/gd.c
	${gd_SRCDIR}/gdfx.c
	${gd_SRCDIR}/gd_security.c
	${gd_SRCDIR}/gd_gd.c
	${gd_SRCDIR}/gd_gd2.c
	${gd_SRCDIR}/gd_io.c
	${gd_SRCDIR}/gd_io_dp.c
	${gd_SRCDIR}/gd_gif_in.c
	${gd_SRCDIR}/gd_gif_out.c
	${gd_SRCDIR}/gd_io_file.c
	${gd_SRCDIR}/gd_io_ss.c
	${gd_SRCDIR}/gd_jpeg.c
	${gd_SRCDIR}/gd_png.c
	${gd_SRCDIR}/gd_ss.c
	${gd_SRCDIR}/gd_topal.c
	${gd_SRCDIR}/gd_wbmp.c
	${gd_SRCDIR}/gdcache.c
	${gd_SRCDIR}/gdfontg.c
	${gd_SRCDIR}/gdfontl.c
	${gd_SRCDIR}/gdfontmb.c
	${gd_SRCDIR}/gdfonts.c
	${gd_SRCDIR}/gdfontt.c
	${gd_SRCDIR}/gdft.c
	${gd_SRCDIR}/gdhelpers.c
	${gd_SRCDIR}/gdkanji.c
	${gd_SRCDIR}/gdxpm.c
	${gd_SRCDIR}/wbmp.c
)
