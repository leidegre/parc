param(
  [string]$Variant = 'debug',
  [switch]$Verbose = $false,
  [string[]]$TestList = @()
)

& tundra2 "win64-msvc-$Variant-default" parc
if ($lastexitcode -ne 0) {
  write-host 'Build failed.'
  exit 1
}

$parc_args = @()
if ($Verbose) {
  $parc_args += '-v'
}

& "t2-output\win64-msvc-$Variant-default\parc.exe" $parc_args $TestList
if ($lastexitcode -ne 0) {
  write-host 'Tests failed.'
  exit 1
}

write-host 'Build and tests passed.'
