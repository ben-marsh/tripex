# Microsoft Developer Studio Project File - Name="Tripex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Tripex - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Tripex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Tripex.mak" CFG="Tripex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Tripex - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Tripex - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Tripex - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TRIPEX_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Ox /Ot /Og /Oi /Oy- /I "..\..\(shared)\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TRIPEX_EXPORTS" /FAc /FR /YX"pch.h" /FD /c
# SUBTRACT CPP /Os
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 d3d8.lib d3dx8.lib winmm.lib dxerr8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /base:"0x67180000" /dll /profile /map /debug /machine:I386 /out:"Release\vis-tx3.dll"

!ELSEIF  "$(CFG)" == "Tripex - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TRIPEX_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /Gm /GX /ZI /I "..\..\(shared)\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TRIPEX_EXPORTS" /FR /YX"pch.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3d8.lib d3dx8.lib winmm.lib dxerr8.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /dll /debug /machine:I386 /out:"Debug\vis-tx3.dll" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Tripex - Win32 Release"
# Name "Tripex - Win32 Debug"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Group "Errors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\error.cpp
# End Source File
# Begin Source File

SOURCE=.\error.h
# End Source File
# End Group
# Begin Group "Config"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCfgItem.cpp
# End Source File
# Begin Source File

SOURCE=.\CCfgItem.h
# End Source File
# Begin Source File

SOURCE=".\config-defaults.cpp"
# End Source File
# Begin Source File

SOURCE=".\config-defaults.h"
# End Source File
# Begin Source File

SOURCE=".\config-variables.cpp"
# End Source File
# Begin Source File

SOURCE=".\config-variables.h"
# End Source File
# Begin Source File

SOURCE=.\config.cpp
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\CTextureItem.cpp
# End Source File
# Begin Source File

SOURCE=.\CTextureItem.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\(shared)\ZArray.h"
# End Source File
# Begin Source File

SOURCE=.\ZBezier.cpp
# End Source File
# Begin Source File

SOURCE=.\ZBezier.h
# End Source File
# Begin Source File

SOURCE=.\ZCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\ZCamera.h
# End Source File
# Begin Source File

SOURCE="..\..\(shared)\ZColour.h"
# End Source File
# Begin Source File

SOURCE="..\..\(shared)\ZColourHSV.h"
# End Source File
# Begin Source File

SOURCE=.\ZDirect3d.cpp
# End Source File
# Begin Source File

SOURCE=.\ZDirect3d.h
# End Source File
# Begin Source File

SOURCE=.\ZEdge.h
# End Source File
# Begin Source File

SOURCE=.\ZFace.h
# End Source File
# Begin Source File

SOURCE=.\ZGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\ZGrid.h
# End Source File
# Begin Source File

SOURCE=.\ZMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\ZMatrix.h
# End Source File
# Begin Source File

SOURCE=.\ZMatrix.inl
# End Source File
# Begin Source File

SOURCE=".\ZObject-Clip.cpp"
# End Source File
# Begin Source File

SOURCE=".\ZObject-Primitives.cpp"
# End Source File
# Begin Source File

SOURCE=.\ZObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ZObject.h
# End Source File
# Begin Source File

SOURCE=.\ZPaletteCanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\ZPaletteCanvas.h
# End Source File
# Begin Source File

SOURCE=.\ZSpriteBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\ZSpriteBuffer.h
# End Source File
# Begin Source File

SOURCE=.\ZTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\ZTexture.h
# End Source File
# Begin Source File

SOURCE=.\ZTextureFont.cpp
# End Source File
# Begin Source File

SOURCE=.\ZTextureFont.h
# End Source File
# Begin Source File

SOURCE=.\ZVector.cpp
# End Source File
# Begin Source File

SOURCE=.\ZVector.h
# End Source File
# Begin Source File

SOURCE=.\ZVector.inl
# End Source File
# End Group
# Begin Group "Effects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\effect-beziercube.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-blank.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-bumpmapping.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-collapsinglightsphere.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-distortion1.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-distortion2.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-dotstar.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-flowmap.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-lightring.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-lightsphere.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-lightstar.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-lighttentacles.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-morphingsphere.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-motionblur1.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-motionblur2.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-motionblur3.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-phased.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-rings.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-spectrum.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-sun.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-tube.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-tunnel.cpp"
# End Source File
# Begin Source File

SOURCE=".\effect-waterglobe.cpp"
# End Source File
# Begin Source File

SOURCE=.\effect.cpp
# End Source File
# Begin Source File

SOURCE=.\effect.h
# End Source File
# Begin Source File

SOURCE=.\ZEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\ZEffect.h
# End Source File
# End Group
# Begin Group "XBMC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Harness.cpp
# End Source File
# Begin Source File

SOURCE=.\xbmc.cpp
# End Source File
# Begin Source File

SOURCE=.\xbmc.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\general.cpp
# End Source File
# Begin Source File

SOURCE=.\general.h
# End Source File
# Begin Source File

SOURCE=.\Misc.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc.h
# End Source File
# Begin Source File

SOURCE=.\pch.cpp
# End Source File
# Begin Source File

SOURCE=.\pch.h
# End Source File
# Begin Source File

SOURCE=.\star.h
# End Source File
# Begin Source File

SOURCE=.\TextureData.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureData.h
# End Source File
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# Begin Source File

SOURCE=.\ZAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\ZAudio.h
# End Source File
# Begin Source File

SOURCE=.\ZFft.cpp
# End Source File
# Begin Source File

SOURCE=.\ZFft.h
# End Source File
# Begin Source File

SOURCE=.\ZPalette.h
# End Source File
# End Target
# End Project
