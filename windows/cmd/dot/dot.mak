# Microsoft Developer Studio Generated NMAKE File, Based on dot.dsp
!IF "$(CFG)" == ""
CFG=dot - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dot - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dot - Win32 Release" && "$(CFG)" != "dot - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dot.mak" CFG="dot - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dot - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dot - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dot - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\dot.exe"


CLEAN :
	-@erase "$(INTDIR)\dot.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dot.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "../../lib/dotgen" /I "../../lib/neatogen" /I "../../lib/fdpgen" /I "../../lib/circogen" /I "../../lib/twopigen" /I "../../lib/common" /I "../../lib/gvc" /I "../.." /I "../../lib/pathplan" /I "../../lib/cdt" /I "../../lib/gd" /I "../../lib/graph" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "MSWIN32" /D "HAVE_CONFIG_H" /D "BUILTINS" /Fp"$(INTDIR)\dot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=pack.lib pathplan.lib plugin.lib gd.lib graph.lib cdt.lib common.lib gvc.lib neatogen.lib twopigen.lib fdpgen.lib circogen.lib dotgen.lib libexpat.lib libexpatw.lib z.lib png.lib jpeg.lib ft.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\dot.pdb" /machine:I386 /out:"$(OUTDIR)\dot.exe" /libpath:"..\..\lib\lib\Release" /libpath:"..\..\third-party\lib" 
LINK32_OBJS= \
	"$(INTDIR)\dot.obj"

"$(OUTDIR)\dot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dot - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\dot.exe"


CLEAN :
	-@erase "$(INTDIR)\dot.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dot.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /Gi /GX /ZI /Od /I "../../lib/dotgen" /I "../../lib/neatogen" /I "../../lib/fdpgen" /I "../../lib/circogen" /I "../../lib/twopigen" /I "../../lib/common" /I "../../lib/gvc" /I "../.." /I "../../lib/pathplan" /I "../../lib/cdt" /I "../../lib/gd" /I "../../lib/graph" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "MSWIN32" /D "HAVE_CONFIG_H" /D "BUILTINS" /Fp"$(INTDIR)\dot.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dot.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=pack.lib pathplan.lib plugin.lib gd.lib graph.lib cdt.lib common.lib gvc.lib neatogen.lib twopigen.lib fdpgen.lib circogen.lib dotgen.lib libexpat.lib libexpatw.lib z.lib png.lib jpeg.lib ft.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /debug /machine:I386 /out:"$(OUTDIR)\dot.exe" /libpath:"..\..\lib\lib\Debug" /libpath:"..\..\third-party\lib" 
LINK32_OBJS= \
	"$(INTDIR)\dot.obj"

"$(OUTDIR)\dot.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dot.dep")
!INCLUDE "dot.dep"
!ELSE 
!MESSAGE Warning: cannot find "dot.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dot - Win32 Release" || "$(CFG)" == "dot - Win32 Debug"
SOURCE=dot.c

"$(INTDIR)\dot.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

