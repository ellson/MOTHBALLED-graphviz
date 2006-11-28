:PACKAGE: graphviz

CC = nativepp=-1 ncc
LDFLAGS = -Yl,-FORCE:MULTIPLE
CCFLAGS = -Bstatic -O2 -Yc,-ML
FEATURE_MS == 1
_MBCS == 1
_WINDOWS == 1
NDEBUG == 1
FEATURE_WIN32 == 1
FEATURE_MINTSIZE == 1

.SOURCE.h : dot2l os/mswin32 ws/mswin32
.SOURCE.c : dot2l os/mswin32
.SOURCE.a : gfx

	/* -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 */
lefty :: lefty.1 lefty.c code.c display.c exec.c io.c \
	gfxview.c internal.c lex.c parse.c str.c tbl.c txtview.c \
	dot2l.c dotlex.c dotparse.c dottrie.c lefty.res -lgfx
	./nld

lefty.res : ws/mswin32/lefty.rc
	rc /l 0x409 /folefty.res /i 'ws\mswin32' /d NDEBUG 'ws\mswin32\lefty.rc' 
