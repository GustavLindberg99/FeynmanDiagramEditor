$MyAppExeName       = "FeynmanDiagramEditor.exe"
$MyAppExePath       = "..\..\build\Release"
$MyWinDeployQtPath  = "..\..\build\Release\windeployqt"
$MyAppOutputDir     = "..\..\build"
$MyAppOutputZip     = "FeynmanDiagramEditor-portable-windows.zip"

If (Test-Path "$MyAppOutputDir\$MyAppOutputZip") {
    Remove-Item "$MyAppOutputDir\$MyAppOutputZip"
}

Compress-Archive `
    -Path "$MyAppExePath\$MyAppExeName","$MyWinDeployQtPath\*" `
    -DestinationPath "$MyAppOutputDir\$MyAppOutputZip" `
    -CompressionLevel Optimal
