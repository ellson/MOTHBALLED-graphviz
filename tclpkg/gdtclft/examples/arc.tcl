#!/bin/sh
# next line is a comment in tcl \
exec tclsh "$0" ${1+"$@"}

package require Gdtclft

set gd [gd create 200 700]
set white [gd color new $gd 255 255 255]
set black [gd color new $gd 0 0 0]

gd arc       $gd $black 100  40 120 40 225 315
gd fillarc   $gd $black 100  80 120 40 225 315
gd openarc   $gd $black 100 120 120 40 225 315

gd chord     $gd $black 100 160 120 40 225 315
gd fillchord $gd $black 100 200 120 40 225 315
gd openchord $gd $black 100 240 120 40 225 315

gd pie       $gd $black 100 280 120 40 225 315
gd fillpie   $gd $black 100 320 120 40 225 315
gd openpie   $gd $black 100 360 120 40 225 315

gd writePNG $gd stdout
