# TODO:
# http://nsis.sourceforge.net/Embedding_other_installers

!include "FileAssociation.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"

!define MY_APP "GcEAD Classic"
!define MY_VERSION "4.6"

name "GcEAD Classic v${MY_VERSION}"
outfile "GcEAD_Classic-${MY_VERSION}-installer.exe"
installDir "$PROGRAMFILES\Syntech\GcEAD_Classic-${MY_VERSION}"
crcCheck on
requestExecutionLevel highest

page components
page directory
page instfiles
uninstPage uninstConfirm
uninstPage instfiles

var arch

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
functionEnd

section "Interface driver"
	MessageBox MB_OK 'You want to install interface'
sectionEnd

section "IDAC driver"
	MessageBox MB_OK 'You want to install IDAC driver'
sectionEnd

section "GcEAD v4.6 (classic)"
	MessageBox MB_OK 'You want to install GcEAD v4.6 (classic)'
sectionEnd
