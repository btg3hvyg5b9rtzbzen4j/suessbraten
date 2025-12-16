Write-Host "Compiling C files..." -ForegroundColor Cyan

$output = gcc (Get-ChildItem *.c | ForEach-Object { $_.FullName }) -o main.exe 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation successful!" -ForegroundColor Green
} else {
    Write-Host "Compilation failed!`n" -ForegroundColor Red
    Write-Host "Errors:`n" -ForegroundColor Yellow
    Write-Host $output -ForegroundColor Red
}

Start-Process main.exe