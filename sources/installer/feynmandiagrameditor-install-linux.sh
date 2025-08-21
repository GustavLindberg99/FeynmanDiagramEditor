#!/usr/bin/bash
MyAppName="FeynmanDiagramEditor"
MyAppExeName="FeynmanDiagramEditor"
MyAppExePath="../../build/Linux"
MyAppMajorVersion="$(grep -Ei '#define MAJORVERSION ([0-9])+' ../version.h | grep -oEi '[0-9]+')"
MyAppMinorVersion="$(grep -Ei '#define MINORVERSION ([0-9])+' ../version.h | grep -oEi '[0-9]+')"
MyAppPatchVersion="$(grep -Ei '#define PATCHVERSION ([0-9])+' ../version.h | grep -oEi '[0-9]+')"
MyAppVersion="${MyAppMajorVersion}.${MyAppMinorVersion}.${MyAppPatchVersion}"
MyAppArchitecture="amd64"
MyAppOutputDir="../../build"
MyAppOutputExe="FeynmanDiagramEditor-install-linux.deb"
MyAppMaintainer="Gustav Lindberg"
MyAppDescription="A program to easily draw Feynman diagrams"

# Create a temporary directory for creating the deb file (this must be in /tmp and not in build to avoid permission issues)
debDir="/tmp/${MyAppExeName}_${MyAppVersion}_${MyAppArchitecture}"
rm -rf "$debDir"
mkdir -p "$debDir/usr/bin"

# Copy the binary
cp "$MyAppExePath/$MyAppExeName" "$debDir/usr/bin"

# Create a file containing info about the package
mkdir "$debDir/DEBIAN"
echo "Package: $MyAppName
Version: $MyAppVersion
Architecture: $MyAppArchitecture
Maintainer: $MyAppMaintainer
Description: $MyAppDescription
" > "$debDir/DEBIAN/control"

# Build the deb file
dpkg-deb --build --root-owner-group "$debDir"

# Move the deb file to the build directory
mv "$debDir.deb" "$MyAppOutputDir/$MyAppOutputExe"
