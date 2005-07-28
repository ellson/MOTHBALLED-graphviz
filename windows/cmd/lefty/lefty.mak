# Microsoft Developer Studio Generated NMAKE File, Based on lefty.dsp
!IF "$(CFG)" == ""
CFG=lefty - Win32 Debug
!MESSAGE No configuration specified. Defaulting to lefty - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "lefty - Win32 Release" && "$(CFG)" != "lefty - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lefty.mak" CFG="lefty - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lefty - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "lefty - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lefty - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\lefty.exe"


CLEAN :
	-@erase "$(INTDIR)\code.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dot2l.obj"
	-@erase "$(INTDIR)\dotlex.obj"
	-@erase "$(INTDIR)\dotparse.obj"
	-@erase "$(INTDIR)\dottrie.obj"
	-@erase "$(INTDIR)\exec.obj"
	-@erase "$(INTDIR)\gfxview.obj"
	-@erase "$(INTDIR)\internal.obj"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\lefty.obj"
	-@erase "$(INTDIR)\lefty.res"
	-@erase "$(INTDIR)\lex.obj"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\str.obj"
	-@erase "$(INTDIR)\tbl.obj"
	-@erase "$(INTDIR)\txtview.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\lefty.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "." /I "..\.." /I ".\dot2l" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MSWIN32" /D "HAVE_CONFIG_H" /D "FEATURE_MS" /D "FEATURE_WIN32" /Fp"$(INTDIR)\lefty.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\lefty.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lefty.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=gfx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\lefty.pdb" /machine:I386 /out:"$(OUTDIR)\lefty.exe" /libpath:"gfx\Release" 
LINK32_OBJS= \
	"$(INTDIR)\code.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\gfxview.obj" \
	"$(INTDIR)\internal.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\lefty.obj" \
	"$(INTDIR)\lex.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\str.obj" \
	"$(INTDIR)\tbl.obj" \
	"$(INTDIR)\txtview.obj" \
	"$(INTDIR)\dot2l.obj" \
	"$(INTDIR)\dotlex.obj" \
	"$(INTDIR)\dotparse.obj" \
	"$(INTDIR)\dottrie.obj" \
	"$(INTDIR)\lefty.res"

"$(OUTDIR)\lefty.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lefty - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\lefty.exe"


CLEAN :
	-@erase "$(INTDIR)\code.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dot2l.obj"
	-@erase "$(INTDIR)\dotlex.obj"
	-@erase "$(INTDIR)\dotparse.obj"
	-@erase "$(INTDIR)\dottrie.obj"
	-@erase "$(INTDIR)\exec.obj"
	-@erase "$(INTDIR)\gfxview.obj"
	-@erase "$(INTDIR)\internal.obj"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\lefty.obj"
	-@erase "$(INTDIR)\lefty.res"
	-@erase "$(INTDIR)\lex.obj"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\str.obj"
	-@erase "$(INTDIR)\tbl.obj"
	-@erase "$(INTDIR)\txtview.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\lefty.exe"
	-@erase "$(OUTDIR)\lefty.ilk"
	-@erase "$(OUTDIR)\lefty.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "." /I "..\.." /I ".\dot2l" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MSWIN32" /D "HAVE_CONFIG_H" /D "FEATURE_MS" /D "FEATURE_WIN32" /Fp"$(INTDIR)\lefty.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\lefty.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lefty.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=gfx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\lefty.pdb" /debug /machine:I386 /out:"$(OUTDIR)\lefty.exe" /pdbtype:sept /libpath:"gfx\Debug" 
LINK32_OBJS= \
	"$(INTDIR)\code.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\gfxview.obj" \
	"$(INTDIR)\internal.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\lefty.obj" \
	"$(INTDIR)\lex.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\str.obj" \
	"$(INTDIR)\tbl.obj" \
	"$(INTDIR)\txtview.obj" \
	"$(INTDIR)\dot2l.obj" \
	"$(INTDIR)\dotlex.obj" \
	"$(INTDIR)\dotparse.obj" \
	"$(INTDIR)\dottrie.obj" \
	"$(INTDIR)\lefty.res"

"$(OUTDIR)\lefty.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
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
!IF EXISTS("lefty.dep")
!INCLUDE "lefty.dep"
!ELSE 
!MESSAGE Warning: cannot find "lefty.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lefty - Win32 Release" || "$(CFG)" == "lefty - Win32 Debug"
SOURCE=.\code.c

"$(INTDIR)\code.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\display.c

"$(INTDIR)\display.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\exec.c

"$(INTDIR)\exec.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gfxview.c

"$(INTDIR)\gfxview.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\internal.c

"$(INTDIR)\internal.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\os\mswin32\io.c

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\lefty.c

"$(INTDIR)\lefty.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\lex.c

"$(INTDIR)\lex.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mem.c

"$(INTDIR)\mem.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\parse.c

"$(INTDIR)\parse.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\str.c

"$(INTDIR)\str.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tbl.c

"$(INTDIR)\tbl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\txtview.c

"$(INTDIR)\txtview.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ws\mswin32\lefty.rc

!IF  "$(CFG)" == "lefty - Win32 Release"


"$(INTDIR)\lefty.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\lefty.res" /i "ws\mswin32" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "lefty - Win32 Debug"


"$(INTDIR)\lefty.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\lefty.res" /i "ws\mswin32" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=.\dot2l\dot2l.c

"$(INTDIR)\dot2l.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dot2l\dotlex.c

"$(INTDIR)\dotlex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dot2l\dotparse.c

"$(INTDIR)\dotparse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dot2l\dottrie.c

"$(INTDIR)\dottrie.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

