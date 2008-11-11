# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in
#

guidir = $(pkgdatadir)/smyrna
gui = smyrna.glade attrs.txt arial.tga template.dot
iconsdir = $(pkgdatadir)/smyrna
icons = add_edge.png AntiselectRect.png AntiselectRectX.png cursor.png \
	deletE_edge.png delete_node.png find_edge.png find_node.png \
	fit_to_screen.png hourglass.png magifier_zoom_out.png magnifier.png \
	magnifier_zoom_in.png move.png new_delete.png new_node.png \
	newzoom.png pan.png select.png selectx.png zoom_in.png \
	zoom_out.png zoom.png \
	fisheye.raw ok.raw ok2.raw ok3.raw pan.raw zoom.raw \
	zoomminus.raw zoomplus.raw rotate.raw \
    dot.PNG glade-3.png lcirco.png ldot.png lfdp.png lneato.png \
    ltwopi.png selectr.png single_select.png

if WITH_SMYRNA
gui_DATA = $(gui)
icons_DATA = $(icons)
endif

EXTRA_DIST = $(gui) $(icons)
