@echo off
rem ===================================================================
rem  run.cmd  ---  build + run the CURRENT file, VS-style
rem
rem  usage : run.cmd "C:\path\to\file.c"        -> run inside this terminal
rem          run.cmd /w "C:\path\to\file.c"     -> run in a NEW black console window
rem  env   : CURRENT_FILE=<path>                (used when no path is given)
rem  support: .c -> gcc   .cpp/.cc/.cxx -> g++   .py -> python
rem  output : <same folder>\<name>.exe
rem ===================================================================
setlocal enabledelayedexpansion

set "WIN="
set "SRC="
for %%A in (%*) do (
    if /i "%%~A"=="/w"      set "WIN=1"
    if /i "%%~A"=="/window" set "WIN=1"
    if /i not "%%~A"=="/w" if /i not "%%~A"=="/window" if not defined SRC set "SRC=%%~fA"
)
if not defined SRC if not "%CURRENT_FILE%"=="" set "SRC=%CURRENT_FILE%"
if "%RUN_IN_WINDOW%"=="1" set "WIN=1"

if "%SRC%"=="" (
    echo [ERROR] no file given. usage: run.cmd [/w] "path\file.c"
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
rem !DIR! ends with a backslash; "D:\x\" is mis-parsed when passed as a
rem PowerShell argument, so keep a backslash-free copy for the launcher.
set "DIRNB=!DIR!"
if "!DIRNB:~-1!"=="\" set "DIRNB=!DIRNB:~0,-1!"

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
    echo                ^(if the exe is in use, close the running program first^)
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
    echo                ^(if the exe is in use, close the running program first^)
    exit /b 1
)
goto :run_exe

:run_exe
if not exist "!OUT!" set "OUT=!DIR!!NAME!.exe"
if not exist "!OUT!" (
    echo [ERROR] executable not found: !OUT!
    exit /b 2
)

rem ---- 1) run in a separate black console window (default for Ctrl+F5) ----
if defined WIN (
    echo [2/2] opening in a NEW console window: !NAME!.exe
    echo       close that window when you are done.
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0run-window.ps1" -Exe "!OUT!" -WorkDir "!DIRNB!" -Title "!NAME!.exe"
    exit /b 0
)

rem ---- 2) run inside this terminal ----
echo [2/2] running !NAME!.exe ...
echo ---------------------------------------
"!OUT!"
set "RC=!ERRORLEVEL!"
echo ---------------------------------------
echo [DONE] exit code = !RC!
exit /b !RC!

:run_py
if defined WIN (
    echo [1/1] opening in a NEW console window: !NAME!.py
    echo       close that window when you are done.
    powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0run-window.ps1" -Exe "python" -Arg "!SRC!" -WorkDir "!DIRNB!" -Title "!NAME!.py"
    exit /b 0
)
echo [1/1] running !NAME!.py ...
echo ---------------------------------------
python "%SRC%"
set "RC=!ERRORLEVEL!"
echo ---------------------------------------
echo [DONE] exit code = !RC!
exit /b !RC!
