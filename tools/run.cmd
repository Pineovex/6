@echo off
rem ===================================================================
rem  run.cmd  ---  build + run the CURRENT file, VS-style (Ctrl+F5)
rem  usage : run.cmd "C:\path\to\file.c"
rem  support: .c -> gcc   .cpp/.cc/.cxx -> g++   .py -> python
rem  output : <same folder>\<name>.exe
rem ===================================================================
setlocal enabledelayedexpansion
set "SRC=%~f1"
if "%SRC%"=="" if not "%CURRENT_FILE%"=="" set "SRC=%CURRENT_FILE%"

if "%SRC%"=="" (
    echo [ERROR] no file given. usage: run.cmd "path\file.c"
    exit /b 2
)
if not exist "%SRC%" (
    echo [ERROR] file not found: %SRC%
    exit /b 2
)

for %%F in ("%SRC%") do (
    set "DIR=%%~dpF"
    set "NAME=%%~nF"
    set "EXT=%%~xF"
)

if /i "!EXT!"==".py"  goto :run_py
if /i "!EXT!"==".c"   goto :build_c
if /i "!EXT!"==".cpp" goto :build_cpp
if /i "!EXT!"==".cc"  goto :build_cpp
if /i "!EXT!"==".cxx" goto :build_cpp
if /i "!EXT!"==".exe" goto :run_exe

echo [ERROR] unsupported file type: !EXT!
echo         supported: .c  .cpp  .cc  .cxx  .py
exit /b 2

:build_c
set "OUT=!DIR!!NAME!.exe"
echo [1/2] compiling !NAME!.c ...
gcc -g -Wall -Wextra -static -o "!OUT!" "%SRC%"
if errorlevel 1 (
    echo.
    echo [BUILD FAILED] fix the errors above, then press Ctrl+F5 again.
    exit /b 1
)
goto :run_exe

:build_cpp
set "OUT=!DIR!!NAME!.exe"
echo [1/2] compiling !NAME!.cpp ...
g++ -std=c++17 -g -O2 -Wall -Wextra -static -o "!OUT!" "%SRC%"
if errorlevel 1 (
    echo.
    echo [BUILD FAILED] fix the errors above, then press Ctrl+F5 again.
    exit /b 1
)
goto :run_exe

:run_exe
if not exist "!OUT!" set "OUT=!DIR!!NAME!.exe"
if not exist "!OUT!" (
    echo [ERROR] executable not found: !OUT!
    exit /b 2
)
echo [2/2] running !NAME!.exe ...
echo ---------------------------------------
"!OUT!"
set "RC=!ERRORLEVEL!"
echo ---------------------------------------
echo [DONE] exit code = !RC!
exit /b !RC!

:run_py
echo [1/1] running !NAME!.py ...
echo ---------------------------------------
python "%SRC%"
set "RC=!ERRORLEVEL!"
echo ---------------------------------------
echo [DONE] exit code = !RC!
exit /b !RC!