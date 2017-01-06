# No dependencies
add_subdirectory(cdt)
add_subdirectory(circogen)
add_subdirectory(dotgen)
add_subdirectory(fdpgen)
add_subdirectory(label)
add_subdirectory(patchwork)
add_subdirectory(pathplan)
add_subdirectory(twopigen)
add_subdirectory(vmalloc)
add_subdirectory(xdot)

# Dependent on: cdt
add_subdirectory(cgraph)
