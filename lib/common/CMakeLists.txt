if (UNIX)
    set(sort_command LC_ALL=C sort)
    set(cat_command cat)
else()
    set(sort_command sort /LOCALE C)
    set(cat_command type)
endif()

file(TO_NATIVE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/color_names TMP)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/color_lib
    COMMAND ${cat_command} brewer_lib svgcolor_lib ${TMP} | ${sort_command} > color_lib
    DEPENDS brewer_lib svgcolor_lib ${CMAKE_CURRENT_SOURCE_DIR}/color_names
)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/colortbl.h
    COMMAND awk -f ${Graphviz_SOURCE_DIR}/awk/colortbl.awk color_lib > colortbl.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/color_lib
)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/brewer_lib
    COMMAND awk -f ${Graphviz_SOURCE_DIR}/awk/brewer.awk ${CMAKE_CURRENT_SOURCE_DIR}/brewer_colors > brewer_lib
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/brewer_colors
)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/svgcolor_lib
    COMMAND awk -f ${Graphviz_SOURCE_DIR}/awk/svgcolor.awk ${CMAKE_CURRENT_SOURCE_DIR}/svgcolor_names > svgcolor_lib
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/svgcolor_names
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ps_font_equiv.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ps_fontmap.txt ps_font_equiv.h
    COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/mksvgfonts.pl fontmap.cfg ${CMAKE_CURRENT_SOURCE_DIR}/ps_font_equiv.txt | ${sort_command} >> ps_font_equiv.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mksvgfonts.pl ${CMAKE_CURRENT_SOURCE_DIR}/ps_fontmap.txt
)

bison_with_replace(htmlparse.y "html")

add_gvlib(graphviz_lib_common
    args.c
    arrows.c
    colxlate.c
    ellipse.c
    emit.c
    geom.c
    globals.c
    htmllex.c
    htmltable.c
    input.c
    intset.c
    labels.c
    memory.c
    ns.c
    output.c
    pointset.c
    postproc.c
    psusershape.c
    routespl.c
    shapes.c
    splines.c
    taper.c
    textspan.c
    timing.c
    utils.c

    ${CMAKE_CURRENT_BINARY_DIR}/htmlparse.h
    ${CMAKE_CURRENT_BINARY_DIR}/htmlparse.c
    ${CMAKE_CURRENT_BINARY_DIR}/colortbl.h
    ${CMAKE_CURRENT_BINARY_DIR}/ps_font_equiv.h
)
execute_process(
    COMMAND sed "s/#undef WITH_CGRAPH/#define WITH_CGRAPH 1/" ${CMAKE_CURRENT_SOURCE_DIR}/types.h.in
    OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/types.h
)
target_include_directories(graphviz_lib_common
    PUBLIC ../gvc # FIXME
    ${CMAKE_CURRENT_BINARY_DIR} # types.h
)
target_link_libraries(graphviz_lib_common
    PRIVATE
    graphviz_lib_label
    graphviz_lib_fdpgen
    graphviz_lib_cgraph
    graphviz_lib_pathplan
    graphviz_lib_xdot

    ${EXPAT_LIBRARIES}
    ${Z_LIBRARY}
)

install(FILES
    arith.h
    color.h
    geom.h
    textspan.h
    usershape.h
    ${CMAKE_CURRENT_BINARY_DIR}/types.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
)
