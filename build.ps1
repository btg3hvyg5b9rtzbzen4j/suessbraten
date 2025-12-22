param (
    [switch]$d
)

Write-Host "Compiling C files... $(if ($d) { '[debug]' })" -ForegroundColor Cyan

$outputFolder = "build"
if (-not (Test-Path $outputFolder)) {
    New-Item -ItemType Directory -Path $outputFolder | Out-Null
}

# Get all C files in current directory
$inputFiles = @(Get-ChildItem -Filter *.c -ErrorAction SilentlyContinue -Recurse | ForEach-Object { $_.FullName })

if ($inputFiles.Count -eq 0) {
    Write-Host 'No C files found!' -ForegroundColor Red
    exit 1
}

# Build gcc arguments
$gccArgs = @($inputFiles)
$gccArgs += "-lGdi32"
$gccArgs += "-mwindows"

if ($d) {
    $gccArgs += "-DDEBUG", "-g"
    $gccArgs += "-o", "$outputFolder/debug.exe"
}
else {
    $gccArgs += "-O2"
    $gccArgs += "-o", "$outputFolder/release.exe"
}

# Compile
$output = & gcc $gccArgs 2>&1

# Check result
if ($LASTEXITCODE -eq 0) {
    Write-Host 'Compilation successful!' -ForegroundColor Green
    Write-Host "Starting application..." -ForegroundColor Cyan
    
    if ($d) {
        & .\build\debug.exe
    }
    else {

        & .\build\release.exe
    }
}
else {
    Write-Host 'Compilation failed!' -ForegroundColor Red
    Write-Host "Errors:`n" -ForegroundColor Yellow
    Write-Host $output -ForegroundColor Red
    exit 1
}