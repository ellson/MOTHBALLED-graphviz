# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

GRAPHS = KW91.gv Latin1.gv NaN.gv abstract.gv alf.gv arrows.gv \
	awilliams.gv clust.gv clust1.gv clust2.gv clust3.gv clust4.gv \
	clust5.gv crazy.gv ctext.gv dfa.gv fig6.gv fsm.gv \
	grammar.gv hashtable.gv honda-tokoro.gv jcctree.gv jsort.gv \
	ldbxtried.gv longflat.gv mike.gv nhg.gv oldarrows.gv \
	pgram.gv pm2way.gv pmpipe.gv polypoly.gv proc3d.gv \
	record2.gv records.gv rowe.gv russian.gv sdh.gv shells.gv states.gv \
	structs.gv table.gv train11.gv trapeziumlr.gv tree.gv triedds.gv \
	try.gv unix.gv unix2.gv viewfile.gv world.gv switch.gv

graphdir = $(pkgdatadir)/graphs
directeddir = $(graphdir)/directed
directed_DATA = $(GRAPHS)

EXTRA_DIST = $(GRAPHS)

CLEANFILES = core *.gv.* *.png .xvpics/* 

test:
	$(top_builddir)/cmd/dot/dot -Tcanon -Tcmap -Tcmapx \
		-Tdia -Tgv -Tfig -Tgif -Thpgl \
		-Tismap -Timap -Tjpg -Tmif -Tmp -Tpcl -Tpdf -Tpic \
		-Tplain -Tplain-ext -Tpng -Tps -Tps2 -Tps:cairo -Tps:lasi -Tsvg \
		-Tsvg:cairo -Tsvgz -Tvrml -Tvtx -Txdot \
		-O $(GRAPHS) 
ins_test:
	dot -Tcanon -Tcmap -Tcmapx \
		-Tdia -Tgv -Tfig -Tgif -Thpgl \
		-Tismap -Timap -Tjpg -Tmif -Tmp -Tpcl -Tpdf -Tpic \
		-Tplain -Tplain-ext -Tpng -Tps -Tps2 -Tps:cairo -Tps:lasi -Tsvg \
		-Tsvg:cairo -Tsvgz -Tvrml -Tvtx -Txdot \
		-O $(GRAPHS) 

#	for i in $(GRAPHS); do \
#		echo "dot $$i"; \
#		$(top_builddir)/cmd/dot/dot \
#			-Tcanon -o$$i.canon \
#			-Tcmap -o$$i.cmap \
#			-Tcmapx -o$$i.cmapx \
#			-Tdia -o$$i.dia \
#			-Tgv -o$$i.gv \
#			-Tfig -o$$i.fig \
#			-Tgd -o$$i.gd \
#			-Tgd2 -o$$i.gd2 \
#			-Tgif -o$$i.gif \
#			-Thpgl -o$$i.hpgl \
#			-Tismap -o$$i.ismap \
#			-Timap -o$$i.imap \
#			-Tjpg -o$$i.jpg \
#			-Tmif -o$$i.mif \
#			-Tmp -o$$i.mp \
#			-Tpcl -o$$i.pcl \
#			-Tpic -o$$i.pic \
#			-Tplain -o$$i.plain \
#			-Tplain-ext -o$$i.plain-ext \
#			-Tpng -o$$i.png \
#			-Tps -o$$i.ps \
#			-Tps2 -o$$i.ps2 \
#			-Tsvg -o$$i.svg \
#			-Tsvgz -o$$i.svgz \
#			-Twbmp -o$$i.wbmp \
#			-Tvrml -o$$i.vrml \
#			-Tvtx -o$$i.vtx \
#			-Txdot -o$$i.xdot \
#				$(top_srcdir)/graphs/directed/$$i; \
#	done

canon:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tcanon -o$$i.canon $(top_srcdir)/graphs/directed/$$i; done

cmap:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tcmap -o$$i.cmap $(top_srcdir)/graphs/directed/$$i; done
cmapx:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tcmapx -o$$i.cmapx $(top_srcdir)/graphs/directed/$$i; done

dia:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tdia -o$$i.dia $(top_srcdir)/graphs/directed/$$i; done

fig:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tfig -o$$i.fig $(top_srcdir)/graphs/directed/$$i; done

gd:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tgd -o$$i.gd $(top_srcdir)/graphs/directed/$$i; done

gd2:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tgd2 -o$$i.gd2 $(top_srcdir)/graphs/directed/$$i; done

gif:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tgif -o$$i.gif $(top_srcdir)/graphs/directed/$$i; done

gv dot:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tgv -o$$i.gv $(top_srcdir)/graphs/directed/$$i; done

hpgl:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Thpgl -o$$i.hpgl $(top_srcdir)/graphs/directed/$$i; done

ismap:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tismap -o$$i.ismap $(top_srcdir)/graphs/directed/$$i; done

imap:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Timap -o$$i.imap $(top_srcdir)/graphs/directed/$$i; done

jpg:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tjpg -o$$i.jpg $(top_srcdir)/graphs/directed/$$i; done

mif:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tmif -o$$i.mif $(top_srcdir)/graphs/directed/$$i; done

mp:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tmp -o$$i.mp $(top_srcdir)/graphs/directed/$$i; done

pcl:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tpcl -o$$i.pcl $(top_srcdir)/graphs/directed/$$i; done

pic:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tpic -o$$i.pic $(top_srcdir)/graphs/directed/$$i; done

plain:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tplain -o$$i.plain $(top_srcdir)/graphs/directed/$$i; done

plain-ext:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tplain-ext -o$$i.plain-ext $(top_srcdir)/graphs/directed/$$i; done

pdf:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tpdf -o$$i.pdf $(top_srcdir)/graphs/directed/$$i; done

png:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tpng -o$$i.png $(top_srcdir)/graphs/directed/$$i; done

png_cg:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tpng:cg -o$$i.cg.png $(top_srcdir)/graphs/directed/$$i; done

ps:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tps -o$$i.ps $(top_srcdir)/graphs/directed/$$i; done

ps_gd:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tps:gd -o$$i.gd.ps $(top_srcdir)/graphs/directed/$$i; done

ps_cairo:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tps:cairo -o$$i.cairo.ps $(top_srcdir)/graphs/directed/$$i; done

ps_lasi:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tps:lasi -o$$i.lasi.ps $(top_srcdir)/graphs/directed/$$i; done

ps2:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tps2 -o$$i.ps2 $(top_srcdir)/graphs/directed/$$i; done

svg:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tsvg -o$$i.svg $(top_srcdir)/graphs/directed/$$i; done

svgz:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tsvgz -o$$i.svgz $(top_srcdir)/graphs/directed/$$i; done

wbmp:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Twbmp -o$$i.wbmp $(top_srcdir)/graphs/directed/$$i; done

vrml:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tvrml -o$$i.vrml $(top_srcdir)/graphs/directed/$$i; done

vtx:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Tvtx -o$$i.vtx $(top_srcdir)/graphs/directed/$$i; done

xdot:
	for i in $(GRAPHS); do echo "dot $$i"; \
		$(top_builddir)/cmd/dot/dot -Txdot -o$$i.xdot $(top_srcdir)/graphs/directed/$$i; done
