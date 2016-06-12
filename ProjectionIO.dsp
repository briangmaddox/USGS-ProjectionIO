# Microsoft Developer Studio Project File - Name="ProjectionIO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ProjectionIO - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ProjectionIO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProjectionIO.mak" CFG="ProjectionIO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProjectionIO - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ProjectionIO - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ProjectionIO - Win32 MThreadDebug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ProjectionIO - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "E:\Project\Project" /I "E:\Project\Project\tiff-v3.5.5\libtiff" /I "E:\Project\Project\libgeotiff-1.1.2" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"E:\Project\Project\Release\ProjectionIO.lib"

!ELSEIF  "$(CFG)" == "ProjectionIO - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "E:\Project\Project" /I "E:\Project\Project\tiff-v3.5.5\libtiff" /I "E:\Project\Project\libgeotiff-1.1.2" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"E:\Project\Project\DebugLibs\ProjectionIO.lib"

!ELSEIF  "$(CFG)" == "ProjectionIO - Win32 MThreadDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProjectionIO___Win32_MThreadDebug"
# PROP BASE Intermediate_Dir "ProjectionIO___Win32_MThreadDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ProjectionIO___Win32_MThreadDebug"
# PROP Intermediate_Dir "ProjectionIO___Win32_MThreadDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /I "E:\Project\Project" /I "E:\Project\Project\tiff-v3.5.5\libtiff" /I "E:\Project\Project\libgeotiff-1.1.2" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "E:\Project\Project" /I "E:\Project\Project\tiff-v3.5.5\libtiff" /I "E:\Project\Project\libgeotiff-1.1.2" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_DLL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"E:\Project\Project\Release\ProjectionIO.lib"
# ADD LIB32 /nologo /out:"E:\Project\Project\Mthread\ProjectionIO.lib"

!ENDIF 

# Begin Target

# Name "ProjectionIO - Win32 Release"
# Name "ProjectionIO - Win32 Debug"
# Name "ProjectionIO - Win32 MThreadDebug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ProjectionIOException.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectionReader.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectionWriter.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ProjectionIOException.h
# End Source File
# Begin Source File

SOURCE=.\ProjectionReader.h
# End Source File
# Begin Source File

SOURCE=.\ProjectionWriter.h
# End Source File
# End Group
# End Target
# End Project
