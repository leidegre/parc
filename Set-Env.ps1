
param ([switch] $devenv = $false)

#
# set PARC_HOME to current folder
#

$env:PARC_HOME = Split-Path $MyInvocation.MyCommand.Path

echo "PROJECT_HOME=$env:PARC_HOME"

if ($devenv) {
    & "$([Environment]::GetEnvironmentVariable('ProgramFiles(x86)'))\Microsoft Visual Studio 11.0\Common7\IDE\devenv.exe" (ls *.sln | select -f 1)
}
