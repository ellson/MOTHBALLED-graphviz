add_library(graphviz_lib_agraph INTERFACE)
target_include_directories(graphviz_lib_cgraph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(graphviz_lib_cgraph
    graphviz_lib_cdt
)

install(FILES
    cgraph.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/
)
