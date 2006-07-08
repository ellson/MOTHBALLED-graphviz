#
# This module finds if RUBY is installed and determines where the include files
# and libraries are. It also determines what the name of the library is. This
# code sets the following variables:
#
#  RUBY_FOUND              = 1 if found, else 0
#  RUBY_INCLUDE_PATH       = path to where ruby.h can be found
#  RUBY_EXECUTABLE         = full path to the ruby binary
#

SET(RUBY_POSSIBLE_INCLUDE_PATHS
	/usr/lib/ruby/1.8/i386-linux
	/usr/lib64/ruby/1.8/x86_64-linux
)
SET(RUBY_POSSIBLE_LIB_PATHS
	/usr/lib
	/usr/lib64
)
FIND_PATH(RUBY_INCLUDE_PATH ruby.h
	${RUBY_POSSIBLE_INCLUDE_PATHS}
)
FIND_LIBRARY(RUBY_LIBRARY
	NAMES ruby1.8
	PATHS ${RUBY_POSSIBLE_LIB_PATHS}
)
FIND_PROGRAM(RUBY_EXECUTABLE
	NAMES ruby1.8
	PATHS /usr/bin /usr/local/bin
)
MARK_AS_ADVANCED(
	RUBY_EXECUTABLE
	RUBY_LIBRARY
	RUBY_INCLUDE_PATH
)
SET(RUBY_INCLUDES ${RUBY_INCLUDE_PATH})
SET(SWIG_FOUND 0)
IF(EXISTS ${SWIG_EXECUTABLE})
	SET(SWIG_FOUND 1)
ENDIF(EXISTS ${SWIG_EXECUTABLE})

