# TODO:
# x associate .ead to GcEad
# ? make sure that double-clicking .ead file opens it
# x install libusb DLL (installdll.bat)
# x install IDAC inf files (installinf.bat)
# x don't install USB driver by default
# x disable installation of USB driver if IDAC8_32.dll or IDAC30.sys.dll exists

!include "FileAssociation.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"

!define MY_APP "GcEad"
!define MY_VERSION "1.2.4"
!define MY_SRC_QT_DLL "C:\QtSDK\Desktop\Qt\4.8.1\mingw\bin"
!define MY_SRC_MINGW_DLL "C:\QtSDK\mingw\bin"
!define MY_SRC_COMMON "..\Installables"

name "GcEad/2012 version ${MY_VERSION}"
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
    ${ElseIf} $0 == "IA64"
        StrCpy $arch "ID64"
    ${ElseIf} $1 == "IA64"
        StrCpy $arch "IA64"
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

section "GcEad/2011"
    setOutPath "$INSTDIR"

    file ${MY_SRC_COMMON}\gpl-3.0.txt
    file ${MY_SRC_COMMON}\idc2fpga.hex
    file ${MY_SRC_COMMON}\idc4fpga.hex
    file ..\View\images\GcEad.ico
    file ${MY_SRC_MINGW_DLL}\libgcc_s_dw2-1.dll
    file ${MY_SRC_MINGW_DLL}\mingwm10.dll
    file ${MY_SRC_QT_DLL}\QtCore4.dll
    file ${MY_SRC_QT_DLL}\QtGui4.dll
    file ${MY_SRC_QT_DLL}\QtSvg4.dll
    file ${MY_SRC_QT_DLL}\QtXml4.dll
    file ..\Installables\Windows\msvcr90.dll
    file ..\..\GcEad-build-desktop\release\GcEad.exe
    file /r /x .svn ..\Installables\Windows\driver

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

;section /o "IDAC USB Drivers"
section /o $secDriverName
    ${If} $arch == "X86"
        file "/oname=$SYSDIR\libusb0.dll" ..\Installables\Windows\driver\x86\libusb0_x86.dll
    ${ElseIf} $arch == "AMD64"
        ${DisableX64FSRedirection}
        setOutPath "$SYSDIR"
        file ..\Installables\Windows\driver\amd64\libusb0.dll
    ${ElseIf} $arch == "IA64"
        ${DisableX64FSRedirection}
        setOutPath "$SYSDIR"
        file ..\Installables\Windows\driver\ia64\libusb0.dll
    ${EndIf}

    setOutPath "$INSTDIR\driver"
    Exec 'rundll32 libusb0.dll,usb_install_driver_np_rundll idac.inf'
sectionEnd

section "Uninstall"
    delete $INSTDIR\gpl-3.0.txt
    delete $INSTDIR\idc2fpga.hex
    delete $INSTDIR\idc4fpga.hex
    delete $INSTDIR\GcEad.ico

    delete $INSTDIR\libgcc_s_dw2-1.dll
    delete $INSTDIR\mingwm10.dll
    delete $INSTDIR\QtCore4.dll
    delete $INSTDIR\QtGui4.dll
    delete $INSTDIR\QtSvg4.dll
    delete $INSTDIR\QtXml4.dll
    delete $INSTDIR\msvcr90.dll
    delete $INSTDIR\GcEad.exe
    rmdir /r $INSTDIR\driver
    rmdir $INSTDIR\uninstall.exe
    rmdir $INSTDIR

    delete "$SMPROGRAMS\Syntech\GcEad\GcEad ${MY_VERSION}.lnk"
    delete "$SMPROGRAMS\Syntech\GcEad\Uninstall GcEad ${MY_VERSION}.lnk"
    rmdir "$SMPROGRAMS\Syntech\GcEad"

    delete "$DESKTOP\GcEad.lnk"

    ${unregisterExtension} ".ead" "GcEad Project File"
sectionEnd
