param (
    [switch]$d
)

Write-Host "Compiling C files... $(if ($d) { '[debug]' })" -ForegroundColor Cyan

# Get all C files in current directory
$inputFiles = @(Get-ChildItem -Filter *.c -ErrorAction SilentlyContinue | ForEach-Object { $_.FullName })

if ($inputFiles.Count -eq 0) {
    Write-Host 'No C files found!' -ForegroundColor Red
    exit 1
}

# Build gcc arguments
$gccArgs = @($inputFiles)
$gccArgs += "-lGdi32"
$gccArgs += "-mwindows"
$gccArgs += "-o", "main.exe"

if ($d) {
    $gccArgs += "-DDEBUG", "-g"
} else {
    $gccArgs += "-O2"
}

# Compile
$output = & gcc $gccArgs 2>&1

# Check result
if ($LASTEXITCODE -eq 0) {
    Write-Host 'Compilation successful!' -ForegroundColor Green
    Write-Host "Starting application..." -ForegroundColor Cyan
    & .\main.exe
} else {
    Write-Host 'Compilation failed!' -ForegroundColor Red
    Write-Host "Errors:`n" -ForegroundColor Yellow
    Write-Host $output -ForegroundColor Red
    exit 1
}