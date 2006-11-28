:PACKAGE: graphviz

all : acyclic bcomps ccomps dijkstra gc gvcolor gvpack gxl2dot nop sccmap \
	tred unflatten

acyclic :: acyclic.c -lgraph -lcdt -lingraphs 
bcomps :: bcomps.c -lgraph -lcdt -lingraphs 
ccomps :: ccomps.c -lgraph -lcdt -lingraphs 
dijkstra :: dijkstra.c -lagraph -lcdt -lcommon -lingraphs 
gc :: gc.c -lgraph -lcdt -lingraphs 
gvcolor :: gvcolor.c colxlate.c -lgraph -lcdt -lingraphs 
gvpack :: gvpack.c -ldotgen -ltwopigen -lcircogen -lfdpgen lgvc -lingraphs \
	-lgvplugin_core -lgvplugin_dot_layout -lgvplugin_neato_layout \
    -lgvplugin_gd -lgvplugin_pango 
gxl2dot :: gxl2dot.c dot2gxl.c cvtgxl.c -lagraph -lgraph -lcdt -lcommon -lingraphs
nop :: nop.c -lagraph -lcdt -lcommon -lingraphs 
sccmap :: sccmap.c -lagraph -lcdt -lcommon -lingraphs 
tred :: tred.c -lgraph -lcdt -lingraphs 
unflatten :: unflatten.c -lagraph -lcdt -lcommon -lingraphs 
