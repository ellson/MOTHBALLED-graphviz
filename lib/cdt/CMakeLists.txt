add_definitions(-D_BLD_cdt)

include_directories(.)

add_library(cdt SHARED
    # Header files
    cdt.h
    dthdr.h

    # Source files
    dtclose.c
    dtdisc.c
    dtextract.c
    dtflatten.c
    dthash.c
    dtlist.c
    dtmethod.c
    dtopen.c
    dtrenew.c
    dtrestore.c
    dtsize.c
    dtstat.c
    dtstrhash.c
    dttree.c
    dtview.c
    dtwalk.c

    # Link definition file for Windows
    cdt.def
)
