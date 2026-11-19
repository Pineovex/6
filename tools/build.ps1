# ===================================================================
#  build.ps1 --- compile ONE source file with the right compiler
#
#  Why this exists
#  ---------------
#  VS Code picks the FIRST debug configuration whose type matches, so
#  for a .cpp file it could well land on the "C:" config.  That config
#  used to run `gcc`, and `gcc` does NOT link libstdc++ -- so any C++
#  file died with:
#      undefined reference to `std::cout'
#      undefined reference to `std::istream::operator>>(int&)'
#      collect2.exe: error: ld returned 1 exit status
#  and the debugger never started, which looks like "cannot debug".
#
#  This script picks the compiler from the file extension instead, so
#  either launch configuration can safely use it as preLaunchTask.
#
#  .c            -> gcc  -g -Wall -Wextra -static
#  .cpp/.cc/.cxx -> g++  -std=c++17 -g -Wall -Wextra -static
#  output        -> <same dir>\<name>.exe
# ===================================================================
param(
    [Parameter(Mandatory = $true)][string]$Src,
    # 调试构建用 -O0，避免变量被优化掉。
    # 注意：不能用 -Debug 这个名字，它和 PowerShell 的公共参数冲突，
    # 会报 "A parameter with the name 'Debug' was defined multiple times".
    [switch]$DebugBuild
)

$ErrorActionPreference = 'Continue'

if (-not (Test-Path -LiteralPath $Src)) {
    Write-Host "[ERROR] file not found: $Src"
    exit 2
}

$item = Get-Item -LiteralPath $Src
$dir = $item.DirectoryName
$name = [System.IO.Path]::GetFileNameWithoutExtension($item.Name)
$ext = $item.Extension.ToLowerInvariant()
$out = Join-Path $dir ($name + '.exe')

switch ($ext) {
    '.c' {
        $compiler = 'gcc'
        $flags = @('-g', '-Wall', '-Wextra', '-static')
    }
    { $_ -in '.cpp', '.cc', '.cxx' } {
        $compiler = 'g++'
        $opt = if ($DebugBuild) { '-O0' } else { '-O2' }
        $flags = @('-std=c++17', '-g', $opt, '-Wall', '-Wextra', '-static')
    }
    default {
        Write-Host "[ERROR] unsupported file type: $ext"
        Write-Host "        supported: .c  .cpp  .cc  .cxx"
        exit 2
    }
}

# -------------------------------------------------------------------
# Kill the old instance before compiling.
#
# Why: Windows refuses to overwrite a running .exe. If the program you
# started last time is still alive -- the classic case is a console app
# blocked in `cin >> x` waiting for input, so the window looks "dead"
# while the process is very much alive -- the linker fails with:
#     ld.exe: cannot open output file xxx.exe: Permission denied
#     collect2.exe: error: ld returned 1 exit status
# and F5 only pops up "preLaunchTask terminated, exit code -1", which
# looks like a broken editor but is really just a locked file.
#
# NOTE: only a process whose image path is exactly this output file is
# killed, so an unrelated program that happens to share the name is safe.
# Keep this file ASCII-only: Windows PowerShell 5.1 reads .ps1 without a
# BOM as ANSI, and non-ASCII bytes can corrupt the parse.
# -------------------------------------------------------------------
foreach ($proc in Get-Process -ErrorAction SilentlyContinue) {
    try {
        if ($proc.Path -and ($proc.Path -eq $out)) {
            Write-Host "[build] stopping old instance (PID $($proc.Id)) that locks the exe"
            Stop-Process -Id $proc.Id -Force -ErrorAction Stop
            Start-Sleep -Milliseconds 300
        }
    } catch {
        # processes we cannot inspect (system ones) are skipped
    }
}

Write-Host "[build] $compiler -> $out"
& $compiler @flags -o $out $Src
$rc = $LASTEXITCODE

if ($rc -ne 0) {
    Write-Host "[BUILD FAILED] exit code = $rc"
    exit $rc
}

Write-Host "[build] OK"
exit 0
