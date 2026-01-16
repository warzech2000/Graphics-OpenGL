# List all available projects that can be built
$assignmentsDir = "D:\Grafika 3D\Base\3d 2\Graphics-OpenGL\src\Assignments"
$projects = @()

# Get all directories in Assignments that have CMakeLists.txt
Get-ChildItem -Path $assignmentsDir -Directory | ForEach-Object {
    $cmakeFile = Join-Path $_.FullName "CMakeLists.txt"
    if (Test-Path $cmakeFile) {
        # Read project name from CMakeLists.txt
        $content = Get-Content $cmakeFile -ErrorAction SilentlyContinue
        foreach ($line in $content) {
            if ($line -match 'project\s*\((\w+)') {
                $projectName = $matches[1]
                $projects += $projectName
                break
            }
        }
    }
}

# Sort projects alphabetically
$projects = $projects | Sort-Object

# Output as numbered list
$index = 1
foreach ($project in $projects) {
    Write-Output "$index|$project"
    $index++
}
