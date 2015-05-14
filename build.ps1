param(
  [string]$Variant = 'debug'
)

& tundra2 "win64-msvc-$Variant-default" parc
if ($lastexitcode -ne 0) {
  write-host 'Build failed.'
  exit 1
}

& "t2-output\win64-msvc-$Variant-default\parc.exe"
if ($lastexitcode -ne 0) {
  write-host 'Tests failed.'
  exit 1
}

write-host 'Build and tests passed.'
