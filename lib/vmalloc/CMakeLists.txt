include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(vmalloc STATIC
    # Header files
    vmalloc.h
    vmhdr.h

    # Source files
    malloc.c
    vmbest.c
    vmclear.c
    vmclose.c
    vmdcheap.c
    vmdebug.c
    vmdisc.c
    vmlast.c
    vmopen.c
    vmpool.c
    vmprivate.c
    vmprofile.c
    vmregion.c
    vmsegment.c
    vmset.c
    vmstat.c
    vmstrdup.c
    vmtrace.c
    vmwalk.c
)
