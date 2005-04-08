# Microsoft Developer Studio Generated NMAKE File, Based on gfx.dsp
!IF "$(CFG)" == ""
CFG=gfx - Win32 Debug
!MESSAGE No configuration specified. Defaulting to gfx - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "gfx - Win32 Release" && "$(CFG)" != "gfx - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gfx.mak" CFG="gfx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gfx - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gfx - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gfx - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\gfx.lib"


CLEAN :
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\g.obj"
	-@erase "$(INTDIR)\garray.obj"
	-@erase "$(INTDIR)\gbutton.obj"
	-@erase "$(INTDIR)\gcanvas.obj"
	-@erase "$(INTDIR)\gcommon.obj"
	-@erase "$(INTDIR)\glabel.obj"
	-@erase "$(INTDIR)\gmenu.obj"
	-@erase "$(INTDIR)\gpcanvas.obj"
	-@erase "$(INTDIR)\gquery.obj"
	-@erase "$(INTDIR)\gscroll.obj"
	-@erase "$(INTDIR)\gtext.obj"
	-@erase "$(INTDIR)\gview.obj"
	-@erase "$(INTDIR)\lefty.res"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\gfx.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I ".." /I "..\ws\mswin32" /I "..\..\.." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MSWIN32" /D "HAVE_CONFIG_H" /D "FEATURE_MS" /D "FEATURE_WIN32" /D "FEATURE_MINTSIZE" /Fp"$(INTDIR)\gfx.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\lefty.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gfx.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\gfx.lib" 
LIB32_OBJS= \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\g.obj" \
	"$(INTDIR)\garray.obj" \
	"$(INTDIR)\gbutton.obj" \
	"$(INTDIR)\gcanvas.obj" \
	"$(INTDIR)\gcommon.obj" \
	"$(INTDIR)\glabel.obj" \
	"$(INTDIR)\gmenu.obj" \
	"$(INTDIR)\gpcanvas.obj" \
	"$(INTDIR)\gquery.obj" \
	"$(INTDIR)\gscroll.obj" \
	"$(INTDIR)\gtext.obj" \
	"$(INTDIR)\gview.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\lefty.res"

"$(OUTDIR)\gfx.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\gfx.lib" "$(OUTDIR)\gfx.bsc"


CLEAN :
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\common.sbr"
	-@erase "$(INTDIR)\g.obj"
	-@erase "$(INTDIR)\g.sbr"
	-@erase "$(INTDIR)\garray.obj"
	-@erase "$(INTDIR)\garray.sbr"
	-@erase "$(INTDIR)\gbutton.obj"
	-@erase "$(INTDIR)\gbutton.sbr"
	-@erase "$(INTDIR)\gcanvas.obj"
	-@erase "$(INTDIR)\gcanvas.sbr"
	-@erase "$(INTDIR)\gcommon.obj"
	-@erase "$(INTDIR)\gcommon.sbr"
	-@erase "$(INTDIR)\glabel.obj"
	-@erase "$(INTDIR)\glabel.sbr"
	-@erase "$(INTDIR)\gmenu.obj"
	-@erase "$(INTDIR)\gmenu.sbr"
	-@erase "$(INTDIR)\gpcanvas.obj"
	-@erase "$(INTDIR)\gpcanvas.sbr"
	-@erase "$(INTDIR)\gquery.obj"
	-@erase "$(INTDIR)\gquery.sbr"
	-@erase "$(INTDIR)\gscroll.obj"
	-@erase "$(INTDIR)\gscroll.sbr"
	-@erase "$(INTDIR)\gtext.obj"
	-@erase "$(INTDIR)\gtext.sbr"
	-@erase "$(INTDIR)\gview.obj"
	-@erase "$(INTDIR)\gview.sbr"
	-@erase "$(INTDIR)\lefty.res"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\mem.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\gfx.bsc"
	-@erase "$(OUTDIR)\gfx.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I ".." /I "..\ws\mswin32" /I "..\..\.." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "MSWIN32" /D "HAVE_CONFIG_H" /D "FEATURE_MS" /D "FEATURE_WIN32" /D "FEATURE_MINTSIZE" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gfx.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\lefty.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gfx.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\common.sbr" \
	"$(INTDIR)\g.sbr" \
	"$(INTDIR)\garray.sbr" \
	"$(INTDIR)\gbutton.sbr" \
	"$(INTDIR)\gcanvas.sbr" \
	"$(INTDIR)\gcommon.sbr" \
	"$(INTDIR)\glabel.sbr" \
	"$(INTDIR)\gmenu.sbr" \
	"$(INTDIR)\gpcanvas.sbr" \
	"$(INTDIR)\gquery.sbr" \
	"$(INTDIR)\gscroll.sbr" \
	"$(INTDIR)\gtext.sbr" \
	"$(INTDIR)\gview.sbr" \
	"$(INTDIR)\mem.sbr"

"$(OUTDIR)\gfx.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\gfx.lib" 
LIB32_OBJS= \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\g.obj" \
	"$(INTDIR)\garray.obj" \
	"$(INTDIR)\gbutton.obj" \
	"$(INTDIR)\gcanvas.obj" \
	"$(INTDIR)\gcommon.obj" \
	"$(INTDIR)\glabel.obj" \
	"$(INTDIR)\gmenu.obj" \
	"$(INTDIR)\gpcanvas.obj" \
	"$(INTDIR)\gquery.obj" \
	"$(INTDIR)\gscroll.obj" \
	"$(INTDIR)\gtext.obj" \
	"$(INTDIR)\gview.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\lefty.res"

"$(OUTDIR)\gfx.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("gfx.dep")
!INCLUDE "gfx.dep"
!ELSE 
!MESSAGE Warning: cannot find "gfx.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gfx - Win32 Release" || "$(CFG)" == "gfx - Win32 Debug"
SOURCE=..\common.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\common.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\common.obj"	"$(INTDIR)\common.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\g.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\g.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\g.obj"	"$(INTDIR)\g.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\garray.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\garray.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\garray.obj"	"$(INTDIR)\garray.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\gbutton.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\gbutton.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\gbutton.obj"	"$(INTDIR)\gbutton.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\gcanvas.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\gcanvas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\gcanvas.obj"	"$(INTDIR)\gcanvas.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\gcommon.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\gcommon.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\gcommon.obj"	"$(INTDIR)\gcommon.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\glabel.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\glabel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\glabel.obj"	"$(INTDIR)\glabel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\gmenu.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\gmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\gmenu.obj"	"$(INTDIR)\gmenu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\gpcanvas.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\gpcanvas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\gpcanvas.obj"	"$(INTDIR)\gpcanvas.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\gquery.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\gquery.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\gquery.obj"	"$(INTDIR)\gquery.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\gscroll.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\gscroll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\gscroll.obj"	"$(INTDIR)\gscroll.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\gtext.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\gtext.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\gtext.obj"	"$(INTDIR)\gtext.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\gview.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\gview.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\gview.obj"	"$(INTDIR)\gview.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\mem.c

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\mem.obj"	"$(INTDIR)\mem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ws\mswin32\lefty.rc

!IF  "$(CFG)" == "gfx - Win32 Release"


"$(INTDIR)\lefty.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\lefty.res" /i "\graphvizCVS\builddaemon\graphviz-win\cmd\lefty\ws\mswin32" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "gfx - Win32 Debug"


"$(INTDIR)\lefty.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\lefty.res" /i "\graphvizCVS\builddaemon\graphviz-win\cmd\lefty\ws\mswin32" /d "_DEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

