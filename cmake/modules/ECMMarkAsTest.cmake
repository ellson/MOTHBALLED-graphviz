#.rst:
# ECMMarkAsTest
# -------------
#
# Marks a target as only being required for tests.
#
# ::
#
#   ecm_mark_as_test(<target1> [<target2> [...]])
#
# This will cause the specified targets to not be built unless either
# BUILD_TESTING is set to ON or the user invokes the ``buildtests`` target.
#
# BUILD_TESTING is created as a cache variable by the CTest module and by the
# :kde-module:`KDECMakeSettings` module.
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2012 Stephen Kelly <steveire@gmail.com>
# Copyright 2012 Alex Neundorf <neundorf@kde.org>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file COPYING-CMAKE-SCRIPTS for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of extra-cmake-modules, substitute the full
#  License text for the above reference.)

if (NOT BUILD_TESTING)
  if(NOT TARGET buildtests)
    add_custom_target(buildtests)
  endif()
endif()

function(ecm_mark_as_test)
  if (NOT BUILD_TESTING)
    foreach(_target ${ARGN})
      set_target_properties(${_target}
                              PROPERTIES
                              EXCLUDE_FROM_ALL TRUE
                           )
      add_dependencies(buildtests ${_target})
    endforeach()
  endif()
endfunction()
