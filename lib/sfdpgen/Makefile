PROG =	sfdp

SRCS =	main.c mmio.c SparseMatrix.c matrix_market.c LinkedList.c QuadTree.c general.c spring_electrical.c Multilevel.c DotIO.c post_process.c sparse_solve.c triangle.c call_tri.c overlap.c red_black_tree.c

OBJS =	main.o mmio.o SparseMatrix.o matrix_market.o LinkedList.o QuadTree.o general.o spring_electrical.o Multilevel.o DotIO.o post_process.o sparse_solve.o triangle.o call_tri.o overlap.o red_black_tree.o


LIBS =	-lm
GVIZ_BUILD_PATH = /Users/north
GVIZ_BUILD_PATH = /home/yifanhu/GraphViz/graphviz2/build

LIBS = -L$(GVIZ_BUILD_PATH)/lib -lgraph -lcdt -lm


CC = cc
#CFLAGS = -g -Wall -DDEBUG -DDEBUG_PRINT -DHAVE_DOT 
#CFLAGS = -O2 -Wall -DDEBUG_PRINT -DHAVE_DOT -I$(GVIZ_BUILD_PATH)/include/graphviz
#CFLAGS = -g -Wall -DDEBUG -DDEBUG_PRINT
#CFLAGS = -O3 -Wall -DDEBUG_PRINT
CFLAGS = -g -DHAVE_DOT -I$(GVIZ_BUILD_PATH)/include/graphviz/ -Wall -DDEBUG_PRINT -DTIME
CFLAGS = -g -pg -DHAVE_DOT -I$(GVIZ_BUILD_PATH)/include/graphviz/ -Wall -DDEBUG_PRINT -DTIME
CFLAGS = -O3 -DHAVE_DOT  -I$(GVIZ_BUILD_PATH)/include/graphviz/ -Wall -DDEBUG_PRINT -DTIME
#CFLAGS = -O3 -Wall 
#CFLAGS = -pg -g -Wall -DDEBUG_PRINT
#CFLAGS = -g -DHAVE_DOT -I$(GVIZ_BUILD_PATH)/include/graphviz/ -Wall -DDEBUG_PRINT -DTIME
CFLAGS = -O3 -DHAVE_DOT  -I$(GVIZ_BUILD_PATH)/include/graphviz/ -Wall -DDEBUG_PRINT -DTIME
FC = f77
FFLAGS = -O
F90 = f90
F90FLAGS = -O
LDFLAGS = -g
LDFLAGS = $(CFLAGS)


all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(PROG) $(OBJS) *.mod

.SUFFIXES: $(SUFFIXES) .f90

.f90.o:
	$(F90) $(F90FLAGS) -c $<

main.o: mmio.h matrix_market.h spring_electrical.h SparseMatrix.h QuadTree.h
dotIO.o: SparseMatrix.h DotIO.h
mmio.o: mmio.h
SparseMatrix.o: SparseMatrix.h general.h
matrix_market.o: matrix_market.h mmio.h
LinkedList.o: general.h
QuadTree.o: general.h LinkedList.h QuadTree.h
general.o: general.h
spring_electrical.o: spring_electrical.h general.h SparseMatrix.h Multilevel.h post_process.h overlap.h
Multilevel.o: general.h Multilevel.h SparseMatrix.h
post_process.o: post_process.h general.h sparse_solve.h call_tri.o
sparse_solve.c: sparse_solve.h SparseMatrix.h general.h
triangle.o: triangle.h
call_tri.o: general.h SparseMatrix.h triangle.h
overlap.o: overlap.h general.h SparseMatrix.h triangle.h post_process.h red_black_tree.h
red_black_tree.o: red_black_tree.h
overlap.o : overlap.h
