# FIXME: Make this implicit (i.e. add it to the library interface)
# include_directories(../lib/gvc)
# set(link_libraries "dotgen;common;xdot;gvc;cgraph;cdt;pathplan;ortho;pack;label;ingraphs")

ecm_add_test(test_staticbuild.c
    LINK_LIBRARIES graphviz_plugin_dot_layout graphviz_lib_gvc)
