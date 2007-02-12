set vc=c:\progra~1\micros~3
set PATH=%PATH%;%vc%\VC98\bin;%vc%\Common\MsDev98\bin;
set LIB=%vc%\VC98\lib
set INCLUDE=%vc%\VC98\include

cd lib
mkdir lib
cd lib
mkdir Release
mkdir Debug
cd ..
cd cdt
nmake /nologo /f cdt.mak CFG="cdt - Win32 Release"
cd ..
cd graph
nmake /nologo /f graph.mak CFG="graph - Win32 Release"
cd ..
cd agraph
nmake /nologo /f agraph.mak CFG="agraph - Win32 Release"
cd ..
cd gd
nmake /nologo /f gd.mak CFG="gd - Win32 Release"
cd ..
cd pathplan
nmake /nologo /f pathplan.mak CFG="pathplan - Win32 Release"
cd ..
cd common
nmake /nologo /f common.mak CFG="common - Win32 Release"
cd ..
cd gvc
nmake /nologo /f gvc.mak CFG="gvc - Win32 Release"
cd ..
cd pack
nmake /nologo /f pack.mak CFG="pack - Win32 Release"
cd ..
cd neatogen
nmake /nologo /f neatogen.mak CFG="neatogen - Win32 Release"
cd ..
cd dotgen
nmake /nologo /f dotgen.mak CFG="dotgen - Win32 Release"
cd ..
cd twopigen
nmake /nologo /f twopigen.mak CFG="twopigen - Win32 Release"
cd ..
cd circogen
nmake /nologo /f circogen.mak CFG="circogen - Win32 Release"
cd ..
cd fdpgen
nmake /nologo /f fdpgen.mak CFG="fdpgen - Win32 Release"
cd ..
cd ingraphs
nmake /nologo /f ingraphs.mak CFG="ingraphs - Win32 Release"
cd ..
cd ..

cd plugin
nmake /nologo /f plugin.mak CFG="plugin - Win32 Release"
cd ..

cd cmd
cd dot
nmake /nologo /f dot.mak CFG="dot - Win32 Release"
cd ..
cd lefty
cd gfx
nmake /nologo /f gfx.mak CFG="gfx - Win32 Release"
cd ..
nmake /nologo /f lefty.mak CFG="lefty - Win32 Release"
cd ..
cd dotty
nmake /nologo /f dotty.mak CFG="dotty - Win32 Release"
cd ..
cd lneato
nmake /nologo /f lneato.mak CFG="lneato - Win32 Release"
cd ..
cd tools
nmake /nologo /f Acyclic.mak CFG="Acyclic - Win32 Release"
nmake /nologo /f ccomps.mak CFG="ccomps - Win32 Release"
nmake /nologo /f gvcolor.mak CFG="gvcolor - Win32 Release"
nmake /nologo /f gc.mak CFG="gc - Win32 Release"
nmake /nologo /f nop.mak CFG="nop - Win32 Release"
nmake /nologo /f sccmap.mak CFG="sccmap - Win32 Release"
nmake /nologo /f tred.mak CFG="tred - Win32 Release"
nmake /nologo /f unflatten.mak CFG="unflatten - Win32 Release"
nmake /nologo /f gxl2dot.mak CFG="gxl2dot - Win32 Release"
nmake /nologo /f dijkstra.mak CFG="dijkstra - Win32 Release"
nmake /nologo /f bcomps.mak CFG="bcomps - Win32 Release"
nmake /nologo /f gvpack.mak CFG="gvpack - Win32 Release"
cd ..
cd ..

