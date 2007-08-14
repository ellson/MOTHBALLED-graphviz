export nativepp=-1
if [[ -z "$INSTALLROOT" ]]
then
  INSTALLROOT is undefined in runconf script
  exit 1
fi

PANGOFLAGS=
LTDLFLAGS=--disable-ltdl 
#SHAREFLAGS="--disable-shared --enable-static "
SHAREFLAGS="--enable-shared --disable-static "
export CPPFLAGS='-g -I/c/gtk/2.0/include -I/c/gtk/2.0/include/freetype2' 
export LDFLAGS='-g -L/c/gtk/2.0/lib'
export CC=ncc 

while getopts :PL c
do
  case $c in
   P )
    export FONTCONFIG_CFLAGS=-I/C/gtk/2.0/include/ 
    export FONTCONFIG_LIBS=-L/C/gtk/2.0/lib 
    export FREETYPE2_CFLAGS=-IC:/gtk/2.0/include/freetype2
    export FREETYPE2_LIBS="-LC:/gtk/2.0/lib -lfreetype -lz"
    export PKG_CONFIG=/c/gtk/2.0/bin/pkg-config 
    export PKG_CONFIG_PATH='C:/gtk/2.0/lib/pkgconfig' 
    export PANGOCAIRO_CFLAGS="-I/C/GTK/2.0/include/cairo -I/C/GTK/2.0/include/pango-1.0 -I/C/GTK/2.0/include/glib-2.0 -I/C/GTK/2.0/LIB/GLIB-2.0/INCLUDE"
    export PANGOCAIRO_LIBS="-L/C/GTK/2.0/lib -lfontconfig  -lfreetype  -ljpeg  -lpng  -lexpat  -lz -lcairo -lpango-1.0 -lpangocairo-1.0 -lgobject-2.0 -lgtk-win32-2.0 -lglib-2.0 -lgdk-win32-2.0 -latk-1.0 -lgdk_pixbuf-2.0"  
    export GTK_CFLAGS="-I/C/GTK/2.0/include -I/C/GTK/2.0/INCLUDE/GTK-2.0 -I/C/GTK/2.0/INCLUDE/CAIRO -I/C/GTK/2.0/include/glib-2.0 -I/C/GTK/2.0/LIB/GLIB-2.0/INCLUDE -I/C/GTK/2.0/LIB/GLIB-2.0/INCLUDE -I/C/GTK/2.0/INCLUDE/PANGO-1.0 -I/C/GTK/2.0/LIB/GTK-2.0/INCLUDE -I/C/GTK/2.0/INCLUDE/ATK-1.0"
    export GTK_LIBS=-L/C/GTK/2.0/lib
    PANGOFLAGS="--with-fontconfig --with-fontconfiglibdir=/C/gtk/2.0/lib --with-fontconfigincludedir=/C/gtk/2.0/include/ --with-pangocairo"
    ;;
   L )
    LTDLFLAGS="--enable-ltdl" 
    SHAREFLAGS="--enable-shared --disable-static "
    ;;
esac
done

echo configure -C --prefix=$INSTALLROOT --with-mylibgd --disable-swig --without-x  --without-tclsh --with-codegens $SHAREFLAGS --with-freetype=/c/gtk/2.0/lib $LTDLFLAGS $PANGOFLAGS
configure -C --prefix=$INSTALLROOT --with-mylibgd --disable-swig --without-x  --without-tclsh --with-codegens $SHAREFLAGS --with-freetype=/c/gtk/2.0/lib $LTDLFLAGS $PANGOFLAGS
