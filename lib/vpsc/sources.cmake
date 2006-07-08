# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${vpsc_SRCDIR}
	${top_SRCDIR}
)

SET(vpsc_SRCS
	${vpsc_SRCDIR}/block.cpp
	${vpsc_SRCDIR}/blocks.cpp
	${vpsc_SRCDIR}/constraint.cpp
	${vpsc_SRCDIR}/generate-constraints.cpp
	${vpsc_SRCDIR}/pairingheap/PairingHeap.cpp
	${vpsc_SRCDIR}/remove_rectangle_overlap.cpp
	${vpsc_SRCDIR}/solve_VPSC.cpp
	${vpsc_SRCDIR}/csolve_VPSC.cpp
	${vpsc_SRCDIR}/variable.cpp
)
