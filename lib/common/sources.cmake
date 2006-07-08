# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${top_SRCDIR}
	${common_SRCDIR}
	${gvc_SRCDIR}
	${pack_SRCDIR}
	${fdpgen_SRCDIR}
	${pathplan_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
	${gd_SRCDIR}
)

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${common_SRCDIR}
	OUTPUT htmlparse.c
	OUTPUT htmlparse.h
	DEPENDS htmlparse.y
	COMMAND yacc -dv htmlparse.y
	COMMAND sed "s/yy/html/g" < y.tab.c > htmlparse.c
	COMMAND sed "s/yy/html/g" < y.tab.h > htmlparse.h
	COMMAND rm y.tab.c y.tab.h y.output
)

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${common_SRCDIR}
	OUTPUT ps.h
	DEPENDS ps.txt
	COMMAND awk -f ${awk_SRCDIR}/stringize.awk ps.txt > ps.h
)

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${common_SRCDIR}
	OUTPUT colortbl.h
	DEPENDS color_names
	DEPENDS brewer_colors
	COMMAND LC_COLLATE=C sort color_names > color_lib
	COMMAND awk -f ${awk_SRCDIR}/brewer.awk brewer_colors >> color_lib
	COMMAND awk -f ${awk_SRCDIR}/colortbl.awk color_lib > colortbl.h
	COMMAND rm color_lib
)

SET(common_base_SRCS
	${common_SRCDIR}/arrows.c
	${common_SRCDIR}/colxlate.c
	${common_SRCDIR}/fontmetrics.c
	${common_SRCDIR}/args.c
	${common_SRCDIR}/memory.c
	${common_SRCDIR}/globals.c
	${common_SRCDIR}/htmllex.c
	${common_SRCDIR}/htmltable.c
	${common_SRCDIR}/input.c
	${common_SRCDIR}/pointset.c
	${common_SRCDIR}/postproc.c
	${common_SRCDIR}/routespl.c
	${common_SRCDIR}/splines.c
	${common_SRCDIR}/psusershape.c
	${common_SRCDIR}/timing.c
	${common_SRCDIR}/labels.c
	${common_SRCDIR}/ns.c
	${common_SRCDIR}/shapes.c
	${common_SRCDIR}/utils.c
	${common_SRCDIR}/geom.c
	${common_SRCDIR}/output.c
	${common_SRCDIR}/emit.c
)

SET(common_codegen_SRCS
	${common_SRCDIR}/diagen.c
	${common_SRCDIR}/figgen.c
	${common_SRCDIR}/hpglgen.c
	${common_SRCDIR}/mifgen.c
	${common_SRCDIR}/mpgen.c
	${common_SRCDIR}/picgen.c
	${common_SRCDIR}/vtxgen.c
	${common_SRCDIR}/xdgen.c
)

set(common_generated_SRCS
	${common_SRCDIR}/colortbl.h
	${common_SRCDIR}/htmlparse.h
	${common_SRCDIR}/htmlparse.c
	${common_SRCDIR}/ps.h
)

set(common_SRCS
	${common_base_SRCS}
	${common_codegen_SRCS}
	${common_generated_SRCS}
)
