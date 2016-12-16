# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=lefty - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to lefty - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "lefty - Win32 Release" && "$(CFG)" != "lefty - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
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
################################################################################
# Begin Project
# PROP Target_Last_Scanned "lefty - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "lefty - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\lefty.exe"

CLEAN : 
	-@erase "$(INTDIR)\code.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dot2l.obj"
	-@erase "$(INTDIR)\dotlex.obj"
	-@erase "$(INTDIR)\dotparse.obj"
	-@erase "$(INTDIR)\dottrie.obj"
	-@erase "$(INTDIR)\exec.obj"
	-@erase "$(INTDIR)\g.obj"
	-@erase "$(INTDIR)\garray.obj"
	-@erase "$(INTDIR)\gbutton.obj"
	-@erase "$(INTDIR)\gcanvas.obj"
	-@erase "$(INTDIR)\gcommon.obj"
	-@erase "$(INTDIR)\gfxview.obj"
	-@erase "$(INTDIR)\glabel.obj"
	-@erase "$(INTDIR)\gmenu.obj"
	-@erase "$(INTDIR)\gpcanvas.obj"
	-@erase "$(INTDIR)\gquery.obj"
	-@erase "$(INTDIR)\gscroll.obj"
	-@erase "$(INTDIR)\gtext.obj"
	-@erase "$(INTDIR)\gview.obj"
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
	-@erase "$(OUTDIR)\lefty.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "MSWIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "." /I "./dot2l" /I ".\ws\mswin32" /D "NDEBUG" /D "HAVENETSCAPE" /D "MSWIN32" /D "_WINDOWS" /D "HAVEDOT" /D "HAVEMSWIN" /D "HAVEMSWIN32" /D LEFTYPATH=\"\\home\\ek\\work\\lib\\lefty\" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "." /I "./dot2l" /I ".\ws\mswin32" /D\
 "NDEBUG" /D "HAVENETSCAPE" /D "MSWIN32" /D "_WINDOWS" /D "HAVEDOT" /D\
 "HAVEMSWIN" /D "HAVEMSWIN32" /D LEFTYPATH=\"\\home\\ek\\work\\lib\\lefty\"\
 /Fp"$(INTDIR)/lefty.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/lefty.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lefty.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/lefty.pdb" /machine:I386 /out:"$(OUTDIR)/lefty.exe" 
LINK32_OBJS= \
	"$(INTDIR)\code.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\dot2l.obj" \
	"$(INTDIR)\dotlex.obj" \
	"$(INTDIR)\dotparse.obj" \
	"$(INTDIR)\dottrie.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\g.obj" \
	"$(INTDIR)\garray.obj" \
	"$(INTDIR)\gbutton.obj" \
	"$(INTDIR)\gcanvas.obj" \
	"$(INTDIR)\gcommon.obj" \
	"$(INTDIR)\gfxview.obj" \
	"$(INTDIR)\glabel.obj" \
	"$(INTDIR)\gmenu.obj" \
	"$(INTDIR)\gpcanvas.obj" \
	"$(INTDIR)\gquery.obj" \
	"$(INTDIR)\gscroll.obj" \
	"$(INTDIR)\gtext.obj" \
	"$(INTDIR)\gview.obj" \
	"$(INTDIR)\internal.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\lefty.obj" \
	"$(INTDIR)\lefty.res" \
	"$(INTDIR)\lex.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\str.obj" \
	"$(INTDIR)\tbl.obj" \
	"$(INTDIR)\txtview.obj"

"$(OUTDIR)\lefty.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lefty - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\lefty.exe"

CLEAN : 
	-@erase "$(INTDIR)\code.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dot2l.obj"
	-@erase "$(INTDIR)\dotlex.obj"
	-@erase "$(INTDIR)\dotparse.obj"
	-@erase "$(INTDIR)\dottrie.obj"
	-@erase "$(INTDIR)\exec.obj"
	-@erase "$(INTDIR)\g.obj"
	-@erase "$(INTDIR)\garray.obj"
	-@erase "$(INTDIR)\gbutton.obj"
	-@erase "$(INTDIR)\gcanvas.obj"
	-@erase "$(INTDIR)\gcommon.obj"
	-@erase "$(INTDIR)\gfxview.obj"
	-@erase "$(INTDIR)\glabel.obj"
	-@erase "$(INTDIR)\gmenu.obj"
	-@erase "$(INTDIR)\gpcanvas.obj"
	-@erase "$(INTDIR)\gquery.obj"
	-@erase "$(INTDIR)\gscroll.obj"
	-@erase "$(INTDIR)\gtext.obj"
	-@erase "$(INTDIR)\gview.obj"
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
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\lefty.exe"
	-@erase "$(OUTDIR)\lefty.ilk"
	-@erase "$(OUTDIR)\lefty.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "MSWIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "." /I "./dot2l" /I ".\ws\mswin32" /D "_DEBUG" /D "HAVENETSCAPE" /D "MSWIN32" /D "_WINDOWS" /D "HAVEDOT" /D "HAVEMSWIN" /D "HAVEMSWIN32" /D LEFTYPATH=\"\\home\\ek\\work\\lib\\lefty\" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "." /I "./dot2l" /I ".\ws\mswin32"\
 /D "_DEBUG" /D "HAVENETSCAPE" /D "MSWIN32" /D "_WINDOWS" /D "HAVEDOT" /D\
 "HAVEMSWIN" /D "HAVEMSWIN32" /D LEFTYPATH=\"\\home\\ek\\work\\lib\\lefty\"\
 /Fp"$(INTDIR)/lefty.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/lefty.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lefty.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/lefty.pdb" /debug /machine:I386 /out:"$(OUTDIR)/lefty.exe" 
LINK32_OBJS= \
	"$(INTDIR)\code.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\dot2l.obj" \
	"$(INTDIR)\dotlex.obj" \
	"$(INTDIR)\dotparse.obj" \
	"$(INTDIR)\dottrie.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\g.obj" \
	"$(INTDIR)\garray.obj" \
	"$(INTDIR)\gbutton.obj" \
	"$(INTDIR)\gcanvas.obj" \
	"$(INTDIR)\gcommon.obj" \
	"$(INTDIR)\gfxview.obj" \
	"$(INTDIR)\glabel.obj" \
	"$(INTDIR)\gmenu.obj" \
	"$(INTDIR)\gpcanvas.obj" \
	"$(INTDIR)\gquery.obj" \
	"$(INTDIR)\gscroll.obj" \
	"$(INTDIR)\gtext.obj" \
	"$(INTDIR)\gview.obj" \
	"$(INTDIR)\internal.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\lefty.obj" \
	"$(INTDIR)\lefty.res" \
	"$(INTDIR)\lex.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\str.obj" \
	"$(INTDIR)\tbl.obj" \
	"$(INTDIR)\txtview.obj"

"$(OUTDIR)\lefty.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "lefty - Win32 Release"
# Name "lefty - Win32 Debug"

!IF  "$(CFG)" == "lefty - Win32 Release"

!ELSEIF  "$(CFG)" == "lefty - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\txtview.c
DEP_CPP_TXTVI=\
	".\code.h"\
	".\common.h"\
	".\exec.h"\
	".\g.h"\
	".\mem.h"\
	".\parse.h"\
	".\str.h"\
	".\tbl.h"\
	".\txtview.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\txtview.obj" : $(SOURCE) $(DEP_CPP_TXTVI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\tbl.c
DEP_CPP_TBL_C=\
	".\code.h"\
	".\common.h"\
	".\mem.h"\
	".\tbl.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\tbl.obj" : $(SOURCE) $(DEP_CPP_TBL_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\str.c
DEP_CPP_STR_C=\
	".\code.h"\
	".\common.h"\
	".\internal.h"\
	".\mem.h"\
	".\str.h"\
	".\tbl.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\str.obj" : $(SOURCE) $(DEP_CPP_STR_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\parse.c
DEP_CPP_PARSE=\
	".\code.h"\
	".\common.h"\
	".\internal.h"\
	".\lex.h"\
	".\mem.h"\
	".\parse.h"\
	".\tbl.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\parse.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mem.c
DEP_CPP_MEM_C=\
	".\common.h"\
	".\mem.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\mem.obj" : $(SOURCE) $(DEP_CPP_MEM_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\lex.c
DEP_CPP_LEX_C=\
	".\common.h"\
	".\lex.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\lex.obj" : $(SOURCE) $(DEP_CPP_LEX_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\lefty.c
DEP_CPP_LEFTY=\
	".\code.h"\
	".\common.h"\
	".\display.h"\
	".\exec.h"\
	".\g.h"\
	".\gfxview.h"\
	".\internal.h"\
	".\leftyio.h"\
	".\mem.h"\
	".\parse.h"\
	".\str.h"\
	".\tbl.h"\
	".\txtview.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_LEFTY=\
	".\gmap.h"\
	".\gmap2l.h"\
	

"$(INTDIR)\lefty.obj" : $(SOURCE) $(DEP_CPP_LEFTY) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\internal.c
DEP_CPP_INTER=\
	".\./dot2l\dot2l.h"\
	".\code.h"\
	".\common.h"\
	".\display.h"\
	".\exec.h"\
	".\g.h"\
	".\gfxview.h"\
	".\internal.h"\
	".\leftyio.h"\
	".\mem.h"\
	".\parse.h"\
	".\tbl.h"\
	".\txtview.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_INTER=\
	".\gmap2l.h"\
	

"$(INTDIR)\internal.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gfxview.c
DEP_CPP_GFXVI=\
	".\code.h"\
	".\colors.txt"\
	".\common.h"\
	".\exec.h"\
	".\g.h"\
	".\gfxview.h"\
	".\leftyio.h"\
	".\mem.h"\
	".\parse.h"\
	".\tbl.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gfxview.obj" : $(SOURCE) $(DEP_CPP_GFXVI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\g.c
DEP_CPP_G_C12=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\g.obj" : $(SOURCE) $(DEP_CPP_G_C12) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\exec.c
DEP_CPP_EXEC_=\
	".\code.h"\
	".\common.h"\
	".\exec.h"\
	".\internal.h"\
	".\mem.h"\
	".\str.h"\
	".\tbl.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\exec.obj" : $(SOURCE) $(DEP_CPP_EXEC_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\display.c
DEP_CPP_DISPL=\
	".\code.h"\
	".\common.h"\
	".\display.h"\
	".\mem.h"\
	".\str.h"\
	".\tbl.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\display.obj" : $(SOURCE) $(DEP_CPP_DISPL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\common.c
DEP_CPP_COMMO=\
	".\common.h"\
	".\g.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\code.c
DEP_CPP_CODE_=\
	".\code.h"\
	".\common.h"\
	".\mem.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\code.obj" : $(SOURCE) $(DEP_CPP_CODE_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dot2l\dottrie.c
DEP_CPP_DOTTR=\
	".\common.h"\
	".\dot2l\triefa.h"\
	".\dotparse.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dottrie.obj" : $(SOURCE) $(DEP_CPP_DOTTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dot2l\dotlex.c
DEP_CPP_DOTLE=\
	".\./dot2l\dot2l.h"\
	".\common.h"\
	".\dot2l\triefa.c"\
	".\dot2l\triefa.h"\
	".\dotparse.h"\
	".\leftyio.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dotlex.obj" : $(SOURCE) $(DEP_CPP_DOTLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dot2l\dot2l.c
DEP_CPP_DOT2L=\
	".\./dot2l\dot2l.h"\
	".\code.h"\
	".\common.h"\
	".\leftyio.h"\
	".\mem.h"\
	".\tbl.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dot2l.obj" : $(SOURCE) $(DEP_CPP_DOT2L) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\os\mswin32\io.c
DEP_CPP_IO_C22=\
	".\common.h"\
	".\g.h"\
	".\leftyio.h"\
	".\mem.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\io.obj" : $(SOURCE) $(DEP_CPP_IO_C22) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\gview.c
DEP_CPP_GVIEW=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gview.obj" : $(SOURCE) $(DEP_CPP_GVIEW) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\gtext.c
DEP_CPP_GTEXT=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gtext.obj" : $(SOURCE) $(DEP_CPP_GTEXT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\gscroll.c
DEP_CPP_GSCRO=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gscroll.obj" : $(SOURCE) $(DEP_CPP_GSCRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\gquery.c
DEP_CPP_GQUER=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gquery.obj" : $(SOURCE) $(DEP_CPP_GQUER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\gpcanvas.c
DEP_CPP_GPCAN=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gpcanvas.obj" : $(SOURCE) $(DEP_CPP_GPCAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\gmenu.c
DEP_CPP_GMENU=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gmenu.obj" : $(SOURCE) $(DEP_CPP_GMENU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\glabel.c
DEP_CPP_GLABE=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\glabel.obj" : $(SOURCE) $(DEP_CPP_GLABE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\gcommon.c
DEP_CPP_GCOMM=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gcommon.obj" : $(SOURCE) $(DEP_CPP_GCOMM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\gcanvas.c
DEP_CPP_GCANV=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gcanvas.obj" : $(SOURCE) $(DEP_CPP_GCANV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\gbutton.c
DEP_CPP_GBUTT=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\gbutton.obj" : $(SOURCE) $(DEP_CPP_GBUTT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\garray.c
DEP_CPP_GARRA=\
	".\common.h"\
	".\g.h"\
	".\mem.h"\
	".\ws\mswin32\gcommon.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\garray.obj" : $(SOURCE) $(DEP_CPP_GARRA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dotparse.c
DEP_CPP_DOTPA=\
	".\./dot2l\dot2l.h"\
	

"$(INTDIR)\dotparse.obj" : $(SOURCE) $(DEP_CPP_DOTPA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ws\mswin32\lefty.rc

!IF  "$(CFG)" == "lefty - Win32 Release"


"$(INTDIR)\lefty.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/lefty.res" /i "ws\mswin32" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "lefty - Win32 Debug"


"$(INTDIR)\lefty.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/lefty.res" /i "ws\mswin32" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
