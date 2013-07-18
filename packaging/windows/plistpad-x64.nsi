# Based on the Example: http://nsis.sourceforge.net/A_simple_installer_with_start_menu_shortcut_and_uninstaller
 
!define BUILDDIR "..\..\build\PlistPad-Desktop_Qt_5_0_2_MSVC2012_64bit-Release\release"
 
# If you change the names "app.exe", "icon.ico", or "license.rtf" you should do a search and replace - they show up in a few places.
# All the other settings can be tweaked by editing the !defines at the top of this script
!define APPNAME "Plist Pad"
!define COMPANYNAME "Plist Pad"
!define DESCRIPTION "A friendly, cross-platform propertly list (Plist) file editor."

# These three must be integers
!define VERSIONMAJOR 0
!define VERSIONMINOR 1
!define VERSIONBUILD 0

# These will be displayed by the "Click here for support information" link in "Add/Remove Programs"
# It is possible to use "mailto:" links in here to open the email client
!define HELPURL "http://www.johnwordsworth.com/plist-pad/" # "Support Information" link
!define UPDATEURL "http://www.johnwordsworth.com/plist-pad/" # "Product Updates" link
!define ABOUTURL "http://www.johnwordsworth.com/" # "Publisher" link
# This is the size (in kB) of all the files copied into "Program Files"

!define INSTALLSIZE 40000
 
RequestExecutionLevel admin ;Require admin rights on NT6+ (When UAC is turned on)
 
InstallDir "$PROGRAMFILES\${COMPANYNAME}\${APPNAME}"
 
# rtf or txt file - remember if it is txt, it must be in the DOS text format (\r\n)
LicenseData "license.rtf"

# This will be in the installer/uninstaller's title bar
Name "${COMPANYNAME} - ${APPNAME}"
Icon "icon.ico"

outFile "plist-pad-x64-installer-${VERSIONMAJOR}-${VERSIONMINOR}-${VERSIONBUILD}.exe"
 
!include LogicLib.nsh
 
# Just three pages - license agreement, install location, and installation
Page license
Page directory
Page instfiles
 
!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Administrator rights required!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend
 
function .onInit
	setShellVarContext all
	!insertmacro VerifyUserIsAdmin
functionEnd
 
section "install"
	# Files for the install directory - to build the installer, these should be in the same directory as the install script (this file)
	# Files added here should be removed by the uninstaller (see section "uninstall")
	setOutPath $INSTDIR
	file "icon.ico"
	file "${BUILDDIR}\PlistPad.exe"
	file "${BUILDDIR}\Qt5Core.dll"
	file "${BUILDDIR}\Qt5Gui.dll"
	file "${BUILDDIR}\Qt5Widgets.dll"
	file "${BUILDDIR}\d3dcompiler_46.dll"
	file "${BUILDDIR}\icudt49.dll"
	file "${BUILDDIR}\icuin49.dll"
	file "${BUILDDIR}\icuuc49.dll"
	file "${BUILDDIR}\libEGL.dll"
	file "${BUILDDIR}\libGLESv2.dll"
	
	setOutPath $INSTDIR\platforms
	file "${BUILDDIR}\platforms\qminimal.dll"
	file "${BUILDDIR}\platforms\qwindows.dll"

	setOutPath $INSTDIR
	
	# Add any other files for the install directory (license files, app data, etc) here
 
	# Uninstaller - See function un.onInit and section "uninstall" for configuration
	writeUninstaller "$INSTDIR\uninstall.exe"
 
	# Start Menu
	createDirectory "$SMPROGRAMS\${COMPANYNAME}"
	createShortCut "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk" "$INSTDIR\PlistPad.exe" "" "$INSTDIR\icon.ico"
 
	# Registry information for add/remove programs
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayName" "${COMPANYNAME} - ${APPNAME} - ${DESCRIPTION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayIcon" "$\"$INSTDIR\icon.ico$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "Publisher" "$\"${COMPANYNAME}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "HelpLink" "$\"${HELPURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "URLUpdateInfo" "$\"${UPDATEURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayVersion" "$\"${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}$\""
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "VersionMajor" ${VERSIONMAJOR}
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "VersionMinor" ${VERSIONMINOR}
	# There is no option for modifying or repairing the install
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoRepair" 1
	# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "EstimatedSize" ${INSTALLSIZE}
sectionEnd
 
# Uninstaller
 
function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanantly remove ${APPNAME}?" IDOK next
		Abort
	next:
	!insertmacro VerifyUserIsAdmin
functionEnd
 
section "uninstall"
 
	# Remove Start Menu launcher
	delete "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk"
	# Try to remove the Start Menu folder - this will only happen if it is empty
	rmDir "$SMPROGRAMS\${COMPANYNAME}"
 
	# Remove files
	delete $INSTDIR\icon.ico
	delete $INSTDIR\PlistPad.exe
	delete $INSTDIR\Qt5Core.dll
	delete $INSTDIR\Qt5Gui.dll
	delete $INSTDIR\Qt5Widgets.dll
	delete $INSTDIR\d3dcompiler_46.dll
	delete $INSTDIR\icudt49.dll
	delete $INSTDIR\icuin49.dll
	delete $INSTDIR\icuuc49.dll
	delete $INSTDIR\libEGL.dll
	delete $INSTDIR\libGLESv2.dll	
	delete $INSTDIR\platforms\qminimal.dll
	delete $INSTDIR\platforms\qwindows.dll
 
	# Always delete uninstaller as the last action
	delete $INSTDIR\uninstall.exe
 
	# Try to remove the install directory - this will only happen if it is empty
	rmDir $INSTDIR\platforms
	rmDir $INSTDIR
 
	# Remove uninstaller information from the registry
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}"
sectionEnd