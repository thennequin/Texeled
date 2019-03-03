@echo off

if not exist "ResourceEmbedder_x64.exe" (
	echo Downloading ResourceEmbedder...
	powershell [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri "https://github.com/thennequin/UtilsCollection/releases/download/Build1/ResourceEmbedder_x64.exe" -OutFile '%~dp0\ResourceEmbedder_x64.exe'
)

"%~dp0\ResourceEmbedder_x64.exe" "%~dp0..\Resources" "%~dp0..\src\Resources"
echo "Finish"
pause