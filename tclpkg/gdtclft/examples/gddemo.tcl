#!/bin/sh
# next line is a comment in tcl \
exec tclsh "$0" ${1+"$@"}

package require Gdtclft

################################################################
#
# gddemo in tcl
#

# open demoin.png or die
if {[catch {open demoin.png r} in]} {
  puts stderr "Can't load source image; this demo is much";
  puts stderr "more impressive if demoin.png is available";
  exit
}

# Create output image 128 x 128
set im_out [gd create 128 128]

# First color is background
set white [gd color new $im_out 255 255 255]

# Set transparent
gd color transparent $im_out $white

# Load demoin.png and paste part of it into the output image.
set im_in [gd createFromPNG $in]
close $in
 
# Copy and shrink
gd copy $im_out $im_in 16 16 0 0 96 96 128 128

# Get some colors
set red [gd color new $im_out 255 0 0]
set green [gd color new $im_out 0 255 0]
set blue [gd color new $im_out 0 0 255]

# Draw a rectangle
gd line $im_out $green 8 8 120 8
gd line $im_out $green 120 8 120 120
gd line $im_out $green 120 120 8 120
gd line $im_out $green 8 120 8 8

# Text
gd text $im_out $red arial 20 0 16 16 hi
gd text $im_out $red arial 20 90 23 23 hi

# Circle
gd arc $im_out $blue 64 64 30 10 0 360

# Arc
gd arc $im_out $blue 64 64 20 20 45 135

# Flood fill
gd fill $im_out $blue 4 4

# Polygon
gd fillpolygon $im_out $green 32 0 0 64 64 64

# Brush. A fairly wild example also involving a line style!
if {$im_in != ""} {
  set brush [gd create 8 8];
  eval [concat gd copy $brush $im_in 0 0 0 0 [gd size $brush] [gd size $im_in]]
  gd brush $im_out $brush
  # Style so they won't overprint each other.
  gd style $im_out "0 0 0 0 0 0 0 1"
  gd line $im_out "styled brushed" 0 0 128 128
}

# Interlace the result for "fade in" in viewers that support it
gd interlace $im_out true

# Write PNG
set out [open demoout.png w]
gd writePNG $im_out $out
close $out
gd destroy $im_out
