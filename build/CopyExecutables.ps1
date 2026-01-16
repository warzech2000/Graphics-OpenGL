# Copy only the Release executables to a clean output folder
$assignmentsDir = "D:\Grafika 3D\Base\3d 2\Graphics-OpenGL\build\src\Assignments"
$outputDir = "D:\Grafika 3D\Base\3d 2\Graphics-OpenGL\build\Binaries"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Copying Release executables..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Create output directory if it doesn't exist
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
    Write-Host "Created output directory: $outputDir" -ForegroundColor Green
}

$copiedCount = 0

Get-ChildItem -Path $assignmentsDir -Directory | ForEach-Object {
    $projName = $_.Name
    $exePath = Join-Path $_.FullName "Release\$projName.exe"
    
    if (Test-Path $exePath) {
        $destPath = Join-Path $outputDir "$projName.exe"
        Copy-Item -Path $exePath -Destination $destPath -Force
        Write-Host "  Copied: $projName.exe" -ForegroundColor Green
        $copiedCount++
    } else {
        Write-Host "  Skipped: $projName (no Release .exe found)" -ForegroundColor Yellow
    }
}

Write-Host ""
if ($copiedCount -gt 0) {
    Write-Host "Total: $copiedCount executables copied to:" -ForegroundColor Green
    Write-Host "  $outputDir" -ForegroundColor Cyan
} else {
    Write-Host "No executables found to copy." -ForegroundColor Yellow
}
Write-Host ""
