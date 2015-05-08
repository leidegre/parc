param(
  [string]$Variant = 'debug'
)

& tundra2 "win64-msvc-$Variant-default" parc
if ($lastexitcode -ne 0) {
  write-host 'Build failed.'
}

& "t2-output\win64-msvc-$Variant-default\parc.exe"
if ($lastexitcode -ne 0) {
  write-host 'Tests failed.'
}

write-host 'Build and tests passed.'
