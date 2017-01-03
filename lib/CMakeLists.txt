# No dependencies
add_subdirectory(cdt)
add_subdirectory(circogen)
add_subdirectory(patchwork)
add_subdirectory(pathplan)
add_subdirectory(twopigen)

# Dependent on: cdt
add_subdirectory(cgraph)
