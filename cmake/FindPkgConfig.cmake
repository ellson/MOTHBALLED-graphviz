#
# This module finds if pkg-config is installed and determines where binary is.
# This # code sets the following variables:
#
#  PKGCONFIG_FOUND              = 1 if found, else 0
#  PKGCONFIG_EXECUTABLE         = full path to the ruby binary
#

FIND_PROGRAM(PKGCONFIG_EXECUTABLE
	NAMES pkg-config
	PATHS /usr/bin /usr/local/bin
)
SET(PKGCONFIG_FOUND 0)
IF(EXISTS ${PKGCONFIG_EXECUTABLE})
	SET(PKGCONFIG_FOUND 1)
ENDIF(EXISTS ${PKGCONFIG_EXECUTABLE})
