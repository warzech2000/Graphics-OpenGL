# Find Ninja executable
$ninjaPath = $null

# Check if in PATH
$ninjaInPath = Get-Command ninja -ErrorAction SilentlyContinue
if ($ninjaInPath) {
    $ninjaPath = $ninjaInPath.Source
} else {
    # Check WinGet packages
    $wingetPath = Join-Path $env:LOCALAPPDATA "Microsoft\WinGet\Packages"
    if (Test-Path $wingetPath) {
        $ninjaDirs = Get-ChildItem -Path $wingetPath -Filter "*Ninja*" -Directory -ErrorAction SilentlyContinue
        foreach ($dir in $ninjaDirs) {
            $ninjaExe = Get-ChildItem -Path $dir.FullName -Recurse -Filter "ninja.exe" -ErrorAction SilentlyContinue | Select-Object -First 1
            if ($ninjaExe) {
                $ninjaPath = $ninjaExe.FullName
                break
            }
        }
    }
    
    # Check Program Files
    if (-not $ninjaPath) {
        $progFilesPath = Join-Path $env:ProgramFiles "Ninja\ninja.exe"
        if (Test-Path $progFilesPath) {
            $ninjaPath = $progFilesPath
        }
    }
}

if ($ninjaPath) {
    Write-Output $ninjaPath
}
