# Microsoft Developer Studio Generated NMAKE File, Based on gvcolor.dsp
!IF "$(CFG)" == ""
CFG=gvcolor - Win32 Debug
!MESSAGE No configuration specified. Defaulting to gvcolor - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "gvcolor - Win32 Release" && "$(CFG)" != "gvcolor - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gvcolor.mak" CFG="gvcolor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gvcolor - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gvcolor - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "gvcolor - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\gvcolor.exe"


CLEAN :
	-@erase "$(INTDIR)\colxlate.obj"
	-@erase "$(INTDIR)\gvcolor.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\gvcolor.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "." /I "../.." /I "../../lib/graph" /I "../../lib/cdt" /I "../../lib/ingraphs" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\gvcolor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gvcolor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=graph.lib cdt.lib ingraphs.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\gvcolor.pdb" /machine:I386 /out:"$(OUTDIR)\gvcolor.exe" /libpath:"../../lib/lib/Release" 
LINK32_OBJS= \
	"$(INTDIR)\colxlate.obj" \
	"$(INTDIR)\gvcolor.obj"

"$(OUTDIR)\gvcolor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gvcolor - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\gvcolor.exe"


CLEAN :
	-@erase "$(INTDIR)\colxlate.obj"
	-@erase "$(INTDIR)\gvcolor.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\gvcolor.exe"
	-@erase "$(OUTDIR)\gvcolor.ilk"
	-@erase "$(OUTDIR)\gvcolor.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "." /I "../.." /I "../../lib/graph" /I "../../lib/cdt" /I "../../lib/ingraphs" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\gvcolor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gvcolor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=graph.lib cdt.lib ingraphs.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\gvcolor.pdb" /debug /machine:I386 /out:"$(OUTDIR)\gvcolor.exe" /pdbtype:sept /libpath:"../../lib/lib/Debug" 
LINK32_OBJS= \
	"$(INTDIR)\colxlate.obj" \
	"$(INTDIR)\gvcolor.obj"

"$(OUTDIR)\gvcolor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("gvcolor.dep")
!INCLUDE "gvcolor.dep"
!ELSE 
!MESSAGE Warning: cannot find "gvcolor.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gvcolor - Win32 Release" || "$(CFG)" == "gvcolor - Win32 Debug"
SOURCE=.\colxlate.c

"$(INTDIR)\colxlate.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gvcolor.c

"$(INTDIR)\gvcolor.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

