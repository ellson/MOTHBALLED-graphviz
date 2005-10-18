# Microsoft Developer Studio Generated NMAKE File, Based on gvpack.dsp
!IF "$(CFG)" == ""
CFG=gvpack - Win32 Debug
!MESSAGE No configuration specified. Defaulting to gvpack - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "gvpack - Win32 Release" && "$(CFG)" != "gvpack - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gvpack.mak" CFG="gvpack - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gvpack - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gvpack - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "gvpack - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\gvpack.exe"


CLEAN :
	-@erase "$(INTDIR)\gvpack.obj"
	-@erase "$(INTDIR)\gvpack_builtins.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\gvpack.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "." /I "../.." /I "../../lib/graph" /I "../../lib/cdt" /I "../../lib/ingraphs" /I "../../lib/pack" /I "../../lib/gvc" /I "../../lib/neatogen" /I "../../lib/common" /I "../../lib/pathplan" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\ccomps.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gvpack.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=gd.lib graph.lib cdt.lib ingraphs.lib pack.lib gvc.lib neatogen.lib circogen.lib twopigen.lib fdpgen.lib common.lib plugin.lib pathplan.lib libexpat.lib libexpatw.lib z.lib png.lib jpeg.lib ft.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\gvpack.pdb" /machine:I386 /out:"$(OUTDIR)\gvpack.exe" /libpath:"../../lib/lib/Release" /libpath:"..\..\third-party\lib" 
LINK32_OBJS= \
	"$(INTDIR)\gvpack.obj" \
	"$(INTDIR)\gvpack_builtins.obj"

"$(OUTDIR)\gvpack.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gvpack - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\gvpack.exe"


CLEAN :
	-@erase "$(INTDIR)\gvpack.obj"
	-@erase "$(INTDIR)\gvpack_builtins.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\gvpack.exe"
	-@erase "$(OUTDIR)\gvpack.ilk"
	-@erase "$(OUTDIR)\gvpack.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "." /I "../.." /I "../../lib/graph" /I "../../lib/cdt" /I "../../lib/ingraphs" /I "../../lib/pack" /I "../../lib/gvc" /I "../../lib/neatogen" /I "../../lib/common" /I "../../lib/pathplan" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\ccomps.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gvpack.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=graph.lib cdt.lib ingraphs.lib pack.lib gd.lib gvc.lib neatogen.lib circogen.lib twopigen.lib fdpgen.lib common.lib plugin.lib pathplan.lib libexpat.lib libexpatw.lib z.lib png.lib jpeg.lib ft.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\gvpack.pdb" /debug /machine:I386 /out:"$(OUTDIR)\gvpack.exe" /pdbtype:sept /libpath:"../../lib/lib/Debug" /libpath:"..\..\third-party\lib" 
LINK32_OBJS= \
	"$(INTDIR)\gvpack.obj" \
	"$(INTDIR)\gvpack_builtins.obj"

"$(OUTDIR)\gvpack.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("gvpack.dep")
!INCLUDE "gvpack.dep"
!ELSE 
!MESSAGE Warning: cannot find "gvpack.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gvpack - Win32 Release" || "$(CFG)" == "gvpack - Win32 Debug"
SOURCE=.\gvpack.c

"$(INTDIR)\gvpack.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gvpack_builtins.c

"$(INTDIR)\gvpack_builtins.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

