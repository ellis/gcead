# TODO:
# associate .ead to GcEad
# make sure that double-clicking .ead file opens it
# install libusb DLL (installdll.bat)
# install IDAC inf files (installinf.bat)
# don't install USB driver by default
# disable installation of USB driver if IDAC8_32.dll or IDAC30.sys.dll exists

!define MY_APP "GcEad"
!define MY_VERSION "1.2.3"
!define MY_SRC_QT_DLL "C:\Qt\2010.05\qt\bin"
!define MY_SRC_COMMON "..\Installables"

name "GcEad/2011 version ${MY_VERSION}"
outfile "GcEad-${MY_VERSION}-windows-installer.exe"
installDir $PROGRAMFILES\Syntech\GcEad-nsis

page components
page directory
page instfiles
uninstPage uninstConfirm
uninstPage instfiles

section "GcEad/2011"
    setOutPath $INSTDIR

    file ${MY_SRC_COMMON}\gpl-3.0.txt
    file ${MY_SRC_COMMON}\idc2fpga.hex
    file ${MY_SRC_COMMON}\idc4fpga.hex
    file ..\View\images\GcEad.ico
    file ${MY_SRC_QT_DLL}\libgcc_s_dw2-1.dll
    file ${MY_SRC_QT_DLL}\mingwm10.dll
    file ${MY_SRC_QT_DLL}\QtCore4.dll
    file ${MY_SRC_QT_DLL}\QtGui4.dll
    file ${MY_SRC_QT_DLL}\QtSvg4.dll
    file ${MY_SRC_QT_DLL}\QtXml4.dll
    file ..\Installables\Windows\msvcr90.dll
    file ..\..\GcEad-build-desktop\release\GcEad.exe
    file /r /x .svn ..\Installables\Windows\driver

    # messagebox MB_OK "Hello world!"
    writeUninstaller $INSTDIR\uninstall.exe"

    createShortcut "$SMPROGRAMS\Syntech\GcEad\GcEad ${MY_VERSION}.lnk" "$INSTDIR\GcEad.exe" "%L" "$INSTDIR\GcEad.ico"
    createShortcut "$SMPROGRAMS\Syntech\GcEad\Uninstall GcEad ${MY_VERSION}.lnk" "$INSTDIR\uninstall.exe"
sectionEnd

section "IDAC USB Drivers"
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
sectionEnd
