#!/bin/sh
# next line is a comment in tcl \
exec tclsh "$0" ${1+"$@"}

package require Gdtclft

################################################################
# Sample gdtcl program  - from gdtclft man page
#
# Create a 64 x 64 image
set im [gd create 64 64]

# Get black and white as colors.  Black is the background color because
# it is allocated first from a new image.

set black [gd color new $im 0 0 0]
set white [gd color new $im 255 255 255]

# Draw a line from upper left to lower right
gd line $im $white 0 0 63 63

# Open a file for writing (Tcl on Unix, at least, doesn't support 'wb' mode)
set out [open test.png w]

# Output the image to the disk file
gd writePNG $im $out

# Close the file
close $out

# Destroy the image in memory
gd destroy $im

