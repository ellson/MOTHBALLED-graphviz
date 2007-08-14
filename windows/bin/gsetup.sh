# Prior to running this scritp two directories should exist and 
# include the files below.

# GraphvizTree : this is where Graphviz Windows release is. 
# GraphvizTree should consist following;
# \bin\
# \docs\
# \graphs\
# \include\
# \lib\
# \Uninstall.exe

# ReleaseFolder : before running the script, this folder should have 
# Setup.exe
# Graphviz.ini
# Selfstbv.exe

# 7z.exe and selfex.exe also should be accessed by the script
# 7z is an open source zip utility that can be downloaded directly 
# from http://www.7-zip.org/
# If all goes well, this script should create a file with the 
# name "Graphviz.exe" in the relase folder

PATH=$PATH:/c/progra~1/7-Zip
GraphvizTree=C:/graphviz/local
ReleaseFolder=C:/graphviz/release

#7z a -tzip -r $ReleaseFolder/graphviz.zip $GraphvizTree/*
#7z a -tzip -r '-x!self*.exe' $ReleaseFolder/graphvizw.zip $ReleaseFolder/*
7z a -tzip -r '-x!7z*.*' '-x!*.bat' $ReleaseFolder/graphviz.zip $GraphvizTree/*
7z a -tzip -r '-x!7z*.*' '-x!*.bat' '-x!self*.exe' $ReleaseFolder/graphvizw.zip $ReleaseFolder/*
#7z a -tzip -r $ReleaseFolder/graphvizw.zip $GraphvizTree/*
#7z a -tzip -r '-x!self*.exe' $ReleaseFolder/graphviz.zip $ReleaseFolder/*
selfex $ReleaseFolder/Selfstbv.exe $ReleaseFolder/graphvizw.zip
rm -f $ReleaseFolder/*.zip
echo ""
