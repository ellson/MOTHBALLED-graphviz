# Microsoft Developer Studio Generated NMAKE File, Based on dot2gxl.dsp
!IF "$(CFG)" == ""
CFG=dot2gxl - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dot2gxl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dot2gxl - Win32 Release" && "$(CFG)" != "dot2gxl - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dot2gxl.mak" CFG="dot2gxl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dot2gxl - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dot2gxl - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "dot2gxl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\dot2gxl.exe"


CLEAN :
	-@erase "$(INTDIR)\cvtgxl.obj"
	-@erase "$(INTDIR)\dot2gxl.obj"
	-@erase "$(INTDIR)\gxl2dot.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dot2gxl.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "../.." /I "." /I "../../lib/agraph" /I "../../lib/ingraphs" /I "../../lib/cdt" /I "../../lib/graph" /I "../../third-party/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_LIBEXPAT" /Fp"$(INTDIR)\dot2gxl.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dot2gxl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib agraph.lib cdt.lib common.lib graph.lib ingraphs.lib libexpat.lib libexpatw.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\dot2gxl.pdb" /machine:I386 /out:"$(OUTDIR)\dot2gxl.exe" /libpath:"../../third-party/lib" /libpath:"../../lib/lib/release" 
LINK32_OBJS= \
	"$(INTDIR)\cvtgxl.obj" \
	"$(INTDIR)\dot2gxl.obj" \
	"$(INTDIR)\gxl2dot.obj"

"$(OUTDIR)\dot2gxl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dot2gxl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\dot2gxl.exe"


CLEAN :
	-@erase "$(INTDIR)\cvtgxl.obj"
	-@erase "$(INTDIR)\dot2gxl.obj"
	-@erase "$(INTDIR)\gxl2dot.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dot2gxl.exe"
	-@erase "$(OUTDIR)\dot2gxl.ilk"
	-@erase "$(OUTDIR)\dot2gxl.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "../.." /I "." /I "../../lib/agraph" /I "../../lib/ingraphs" /I "../../lib/cdt" /I "../../lib/graph" /I "../../third-party/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_LIBEXPAT" /Fp"$(INTDIR)\dot2gxl.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dot2gxl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib agraph.lib cdt.lib common.lib graph.lib ingraphs.lib libexpat.lib libexpatw.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\dot2gxl.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dot2gxl.exe" /pdbtype:sept /libpath:"../../third-party/lib" /libpath:"../../lib/lib/debug" 
LINK32_OBJS= \
	"$(INTDIR)\cvtgxl.obj" \
	"$(INTDIR)\dot2gxl.obj" \
	"$(INTDIR)\gxl2dot.obj"

"$(OUTDIR)\dot2gxl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("dot2gxl.dep")
!INCLUDE "dot2gxl.dep"
!ELSE 
!MESSAGE Warning: cannot find "dot2gxl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dot2gxl - Win32 Release" || "$(CFG)" == "dot2gxl - Win32 Debug"
SOURCE=.\cvtgxl.c

"$(INTDIR)\cvtgxl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dot2gxl.c

"$(INTDIR)\dot2gxl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gxl2dot.c

"$(INTDIR)\gxl2dot.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

