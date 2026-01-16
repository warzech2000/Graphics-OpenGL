# Cleanup script for intermediate build files
$assignmentsDir = "D:\Grafika 3D\Base\3d 2\Graphics-OpenGL\build\src\Assignments"
$totalRemoved = 0
$configs = @('Release', 'Debug')

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Cleaning up intermediate build files..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Get-ChildItem -Path $assignmentsDir -Directory | ForEach-Object {
    $projName = $_.Name
    $projectRemoved = 0
    
    foreach ($config in $configs) {
        $buildDir = Join-Path $_.FullName "$projName.dir\$config"
        if (Test-Path $buildDir) {
            # Remove .obj files
            $files = Get-ChildItem -Path $buildDir -Filter '*.obj' -File -ErrorAction SilentlyContinue
            if ($files) { 
                $count = $files.Count
                $files | Remove-Item -Force -ErrorAction SilentlyContinue
                Write-Host "  ${projName} ($config): Removed $count .obj files" -ForegroundColor Gray
                $projectRemoved += $count
            }
            
            # Remove .ilk files
            $files = Get-ChildItem -Path $buildDir -Filter '*.ilk' -File -ErrorAction SilentlyContinue
            if ($files) { 
                $count = $files.Count
                $files | Remove-Item -Force -ErrorAction SilentlyContinue
                Write-Host "  ${projName} ($config): Removed $count .ilk files" -ForegroundColor Gray
                $projectRemoved += $count
            }
            
            # Remove .exe.recipe files
            $files = Get-ChildItem -Path $buildDir -Filter '*.exe.recipe' -File -ErrorAction SilentlyContinue
            if ($files) { 
                $count = $files.Count
                $files | Remove-Item -Force -ErrorAction SilentlyContinue
                Write-Host "  ${projName} ($config): Removed $count .exe.recipe files" -ForegroundColor Gray
                $projectRemoved += $count
            }
            
            # Remove .pdb files (debug symbols)
            $files = Get-ChildItem -Path $buildDir -Filter '*.pdb' -File -ErrorAction SilentlyContinue
            if ($files) { 
                $count = $files.Count
                $files | Remove-Item -Force -ErrorAction SilentlyContinue
                Write-Host "  ${projName} ($config): Removed $count .pdb files" -ForegroundColor Gray
                $projectRemoved += $count
            }
            
            # Remove .tlog directories
            $dirs = Get-ChildItem -Path $buildDir -Directory -Filter '*.tlog' -ErrorAction SilentlyContinue
            if ($dirs) { 
                $count = $dirs.Count
                $dirs | Remove-Item -Recurse -Force -ErrorAction SilentlyContinue
                Write-Host "  ${projName} ($config): Removed $count .tlog directories" -ForegroundColor Gray
                $projectRemoved += $count
            }
            
            # Remove any .tlog files
            $files = Get-ChildItem -Path $buildDir -Filter '*.tlog' -File -ErrorAction SilentlyContinue
            if ($files) { 
                $count = $files.Count
                $files | Remove-Item -Force -ErrorAction SilentlyContinue
                Write-Host "  ${projName} ($config): Removed $count .tlog files" -ForegroundColor Gray
                $projectRemoved += $count
            }
        }
    }
    
    # Also remove entire Debug folder if it exists (since we only build Release)
    $debugExeDir = Join-Path $_.FullName "Debug"
    if (Test-Path $debugExeDir) {
        $exeFiles = Get-ChildItem -Path $debugExeDir -Filter '*.exe' -File -ErrorAction SilentlyContinue
        if ($exeFiles) {
            $count = $exeFiles.Count
            $exeFiles | Remove-Item -Force -ErrorAction SilentlyContinue
            Write-Host "  ${projName}: Removed $count Debug .exe files" -ForegroundColor Yellow
            $projectRemoved += $count
        }
    }
    
    if ($projectRemoved -gt 0) {
        Write-Host "  ${projName}: Total $projectRemoved items removed" -ForegroundColor Green
        $totalRemoved += $projectRemoved
    }
}

Write-Host ""
if ($totalRemoved -eq 0) {
    Write-Host "No intermediate files found to clean." -ForegroundColor Yellow
} else {
    Write-Host "Total: $totalRemoved intermediate files/directories removed." -ForegroundColor Green
}
Write-Host ""
