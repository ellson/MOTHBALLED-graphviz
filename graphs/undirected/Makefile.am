# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

GRAPHS = ER.dot ngk10_4.dot process.dot Heawood.dot Petersen.dot

graphdir = $(pkgdatadir)/graphs
undirecteddir = $(graphdir)/undirected
undirected_DATA = $(GRAPHS)

EXTRA_DIST = $(GRAPHS)

CLEANFILES = core *.dot.* *.png .xvpics/* 

test:
	for i in $(GRAPHS); do \
		echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato \
			-Tcanon -o$$i.canon \
			-Tcmap -o$$i.cmap \
			-Tcmapx -o$$i.cmapx \
			-Tdia -o$$i.dia \
			-Tdot -o$$i.dot \
			-Tfig -o$$i.fig \
			-Tgd -o$$i.gd \
			-Tgd2 -o$$i.gd2 \
			-Tgif -o$$i.gif \
			-Thpgl -o$$i.hpgl \
			-Tismap -o$$i.ismap \
			-Timap -o$$i.imap \
			-Tjpg -o$$i.jpg \
			-Tmif -o$$i.mif \
			-Tmp -o$$i.mp \
			-Tpcl -o$$i.pcl \
			-Tpic -o$$i.pic \
			-Tplain -o$$i.plain \
			-Tplain-ext -o$$i.plain-ext \
			-Tpng -o$$i.png \
			-Tps -o$$i.ps \
			-Tps2 -o$$i.ps2 \
			-Tsvg -o$$i.svg \
			-Tsvgz -o$$i.svgz \
			-Twbmp -o$$i.wbmp \
			-Tvrml -o$$i.vrml \
			-Tvtx -o$$i.vtx \
			-Txdot -o$$i.xdot \
				$(top_srcdir)/graphs/undirected/$$i; \
	done

canon:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tcanon -o$$i.canon $(top_srcdir)/graphs/undirected/$$i; done

cmap:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tcmap -o$$i.cmap $(top_srcdir)/graphs/undirected/$$i; done

cmapx:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tcmapx -o$$i.cmapx $(top_srcdir)/graphs/undirected/$$i; done

dia:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tdia -o$$i.dia $(top_srcdir)/graphs/undirected/$$i; done

dot:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tdot -o$$i.dot $(top_srcdir)/graphs/undirected/$$i; done

fig:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tfig -o$$i.fig $(top_srcdir)/graphs/undirected/$$i; done

gd:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tgd -o$$i.gd $(top_srcdir)/graphs/undirected/$$i; done

gd2:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tgd2 -o$$i.gd2 $(top_srcdir)/graphs/undirected/$$i; done

gif:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tgif -o$$i.gif $(top_srcdir)/graphs/undirected/$$i; done

hpgl:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Thpgl -o$$i.hpgl $(top_srcdir)/graphs/undirected/$$i; done

ismap:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tismap -o$$i.ismap $(top_srcdir)/graphs/undirected/$$i; done

imap:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Timap -o$$i.imap $(top_srcdir)/graphs/undirected/$$i; done

jpg:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tjpg -o$$i.jpg $(top_srcdir)/graphs/undirected/$$i; done

mif:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tmif -o$$i.mif $(top_srcdir)/graphs/undirected/$$i; done

mp:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tmp -o$$i.mp $(top_srcdir)/graphs/undirected/$$i; done

pcl:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tpcl -o$$i.pcl $(top_srcdir)/graphs/undirected/$$i; done

pic:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tpic -o$$i.pic $(top_srcdir)/graphs/undirected/$$i; done

plain:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tplain -o$$i.plain $(top_srcdir)/graphs/undirected/$$i; done

plain-ext:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tplain-ext -o$$i.plain-ext $(top_srcdir)/graphs/undirected/$$i; done

png:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tpng -o$$i.png $(top_srcdir)/graphs/undirected/$$i; done

ps:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tps -o$$i.ps $(top_srcdir)/graphs/undirected/$$i; done

ps2:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tps2 -o$$i.ps2 $(top_srcdir)/graphs/undirected/$$i; done

svg:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tsvg -o$$i.svg $(top_srcdir)/graphs/undirected/$$i; done

svgz:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tsvgz -o$$i.svgz $(top_srcdir)/graphs/undirected/$$i; done

wbmp:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Twbmp -o$$i.wbmp $(top_srcdir)/graphs/undirected/$$i; done

vrml:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tvrml -o$$i.vrml $(top_srcdir)/graphs/undirected/$$i; done

vtx:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Tvtx -o$$i.vtx $(top_srcdir)/graphs/undirected/$$i; done

xdot:
	for i in $(GRAPHS); do echo "neato $$i"; \
		$(top_builddir)/cmd/dot/dot -Kneato -Txdot -o$$i.xdot $(top_srcdir)/graphs/undirected/$$i; done
