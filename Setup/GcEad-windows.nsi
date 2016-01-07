# TODO:
# x associate .ead to GcEad
# ? make sure that double-clicking .ead file opens it
# x install libusb DLL (installdll.bat)
# x install IDAC inf files (installinf.bat)
# x don't install USB driver by default
# x disable installation of USB driver if IDAC8_32.dll or IDAC30.sys.dll exists
# x select drivers for installation by default

!include "FileAssociation.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"

!define MY_APP "GcEad"
!define MY_VERSION "1.2.5"
!define MY_SRC_DLL "C:\Qt\5.5\mingw492_32\bin"
!define MY_SRC_PLUGINS "C:\Qt\5.5\mingw492_32\plugins"
!define MY_SRC_COMMON "..\Installables"
!define MY_SRC_DEPLOY "..\Installables\Windows\windeployqt"

name "GcEad/2015 version ${MY_VERSION}"
outfile "GcEad-${MY_VERSION}-windows-installer.exe"
installDir "$PROGRAMFILES\Syntech\GcEad-${MY_VERSION}"
crcCheck on
requestExecutionLevel highest

page components
page directory
page instfiles
uninstPage uninstConfirm
uninstPage instfiles

var arch
var secDriverName

function .onInit
    readEnvStr $0 PROCESSOR_ARCHITECTURE
    readEnvStr $1 PROCESSOR_ARCHITEW6432
    ${If} $0 == "AMD64"
        StrCpy $arch "AMD64"
    ${ElseIf} $1 == "AMD64"
        StrCpy $arch "AMD64"
    ${Else}
        StrCpy $arch "X86"
    ${EndIf}

    ;messageBox MB_OK "0: $0, 1: $1, Arch: $arch"

    ${If} $arch == "X86"
        ${If} ${FileExists} "$SYSDIR\IDAC8_32.dll"
            StrCpy $secDriverName ""
        ${ElseIf} ${FileExists} "$SYSDIR\IDAC30.sys.dll"
            StrCpy $secDriverName ""
        ${Else}
            StrCpy $secDriverName "IDAC USB Drivers"
        ${EndIf}
    ${Else}
        StrCpy $secDriverName "IDAC USB Drivers"
    ${EndIf}
functionEnd

section "GcEad/2015"
    setOutPath "$INSTDIR"

    file ${MY_SRC_COMMON}\gpl-3.0.txt
    file ${MY_SRC_COMMON}\idc2fpga.hex
    file ${MY_SRC_COMMON}\idc4fpga.hex
    file ..\View\images\GcEad.ico
    file ${MY_SRC_DEPLOY}\*.dll
    file /r ${MY_SRC_DEPLOY}\iconengines
    file /r ${MY_SRC_DEPLOY}\imageformats
    file /r ${MY_SRC_DEPLOY}\platforms
    file /r ${MY_SRC_DEPLOY}\printsupport
    file /r ${MY_SRC_DEPLOY}\translations
    file ..\..\build-GcEad-Desktop_Qt_5_5_1_MinGW_32bit-Release\View\release\GcEad.exe
    file /r ..\Installables\Windows\driver

    # messagebox MB_OK "Hello world!"
    writeUninstaller $INSTDIR\uninstall.exe"

    ; Create start-menu shortcut
    createDirectory "$SMPROGRAMS\Syntech\GcEad"
    createShortcut "$SMPROGRAMS\Syntech\GcEad\GcEad ${MY_VERSION}.lnk" "$INSTDIR\GcEad.exe" "" "$INSTDIR\GcEad.ico"
    createShortcut "$SMPROGRAMS\Syntech\GcEad\Uninstall GcEad ${MY_VERSION}.lnk" "$INSTDIR\uninstall.exe"

    ; Create desktop shortcut
    createShortcut "$DESKTOP\GcEad.lnk" "$INSTDIR\GcEad.exe" "" "$INSTDIR\GcEad.ico"

    ${registerExtension} "$INSTDIR\GcEad.exe" ".ead" "GcEad Project File"
sectionEnd

section "" secLibusbK
    ; GcEad.exe seems to require the x86 version of the lib in its same directory
    file "/oname=$INSTDIR\libusbK.dll" ..\Installables\Windows\driver\x86\libusbK_x86.dll

;    sectionIn RO ; flag this section as required
;    ${If} $arch == "X86"
;        file "/oname=$SYSDIR\libusbK.dll" ..\Installables\Windows\driver\x86\libusbK.dll
;    ${ElseIf} $arch == "AMD64"
;        ${DisableX64FSRedirection}
;        setOutPath "$SYSDIR"
;        file ..\Installables\Windows\driver\amd64\libusbK.dll
;    ${EndIf}
sectionEnd

;section /o "IDAC USB Drivers"
section $secDriverName secIdacDrivers
    setOutPath "$INSTDIR\driver"
    Exec 'InstallDriver.exe'
    ;Exec 'rundll32 libusbK.dll,usb_install_driver_np_rundll idac4.inf'
sectionEnd

section "Uninstall"
    delete $INSTDIR\gpl-3.0.txt
    delete $INSTDIR\idc2fpga.hex
    delete $INSTDIR\idc4fpga.hex
    delete $INSTDIR\GcEad.ico
    delete $INSTDIR\*.dll
    delete $INSTDIR\GcEad.exe
    rmdir /r $INSTDIR\driver
    rmdir /r $INSTDIR\iconengines
    rmdir /r $INSTDIR\imageformats
    rmdir /r $INSTDIR\platforms
    rmdir /r $INSTDIR\printsupport
    rmdir /r $INSTDIR\translations
    rmdir $INSTDIR\uninstall.exe
    rmdir $INSTDIR

    delete "$SMPROGRAMS\Syntech\GcEad\GcEad ${MY_VERSION}.lnk"
    delete "$SMPROGRAMS\Syntech\GcEad\Uninstall GcEad ${MY_VERSION}.lnk"
    rmdir "$SMPROGRAMS\Syntech\GcEad"

    delete "$DESKTOP\GcEad.lnk"

    ${unregisterExtension} ".ead" "GcEad Project File"
sectionEnd
