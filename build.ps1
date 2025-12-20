Write-Host "Compiling C files..." -ForegroundColor Cyan

$output = gcc (Get-ChildItem *.c | ForEach-Object { $_.FullName }) -mwindows -lGdi32 -o main.exe 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation successful!" -ForegroundColor Green
    Start-Process main.exe
} else {
    Write-Host "Compilation failed!`n" -ForegroundColor Red
    Write-Host "Errors:`n" -ForegroundColor Yellow
    Write-Host $output -ForegroundColor Red
}
