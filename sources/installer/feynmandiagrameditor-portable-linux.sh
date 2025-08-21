#!/usr/bin/bash
MyAppExeName="FeynmanDiagramEditor"
MyAppExePath="../../build/Linux"
MyAppOutputDir="../../build"
MyAppOutputZip="FeynmanDiagramEditor-portable-linux.zip"

rm "$MyAppOutputDir/$MyAppOutputZip"
zip -j "$MyAppOutputDir/$MyAppOutputZip" "$MyAppExePath/$MyAppExeName"
