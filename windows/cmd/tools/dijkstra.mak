# Microsoft Developer Studio Generated NMAKE File, Based on dijkstra.dsp
!IF "$(CFG)" == ""
CFG=dijkstra - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dijkstra - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dijkstra - Win32 Release" && "$(CFG)" != "dijkstra - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dijkstra.mak" CFG="dijkstra - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dijkstra - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dijkstra - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "dijkstra - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\dijkstra.exe"


CLEAN :
	-@erase "$(INTDIR)\dijkstra.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dijkstra.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "." /I "../.." /I "../../lib/agraph" /I "../../lib/cdt" /I "../../lib/ingraphs" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\dijkstra.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dijkstra.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=agraph.lib cdt.lib common.lib ingraphs.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\dijkstra.pdb" /machine:I386 /out:"$(OUTDIR)\dijkstra.exe" /libpath:"../../lib/lib/Release" 
LINK32_OBJS= \
	"$(INTDIR)\dijkstra.obj"

"$(OUTDIR)\dijkstra.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dijkstra - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\dijkstra.exe"


CLEAN :
	-@erase "$(INTDIR)\dijkstra.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dijkstra.exe"
	-@erase "$(OUTDIR)\dijkstra.ilk"
	-@erase "$(OUTDIR)\dijkstra.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "." /I "../.." /I "../../lib/agraph" /I "../../lib/cdt" /I "../../lib/ingraphs" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\dijkstra.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dijkstra.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=agraph.lib cdt.lib common.lib ingraphs.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\dijkstra.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dijkstra.exe" /pdbtype:sept /libpath:"../../lib/lib/Debug" 
LINK32_OBJS= \
	"$(INTDIR)\dijkstra.obj"

"$(OUTDIR)\dijkstra.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("dijkstra.dep")
!INCLUDE "dijkstra.dep"
!ELSE 
!MESSAGE Warning: cannot find "dijkstra.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dijkstra - Win32 Release" || "$(CFG)" == "dijkstra - Win32 Debug"
SOURCE=.\dijkstra.c

"$(INTDIR)\dijkstra.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

