# ===================================================================
#  run-window.ps1 --- open a program in its OWN black console window
#  called by tools\run.cmd  (run.cmd /w ...)
#
#  Why not "start"?  Because a child of `start` inherits our
#  stdout/stderr handles, so the VS Code task would stay "running"
#  until the program exits.  Start-Process detaches completely, so the
#  task finishes the moment the window opens.
#
#  CHINESE OUTPUT (important)
#  --------------------------
#  .c/.cpp sources are UTF-8 and gcc/g++ 14+ default to -fexec-charset=UTF-8,
#  so the program writes UTF-8 bytes to stdout.  A fresh cmd window starts
#  on code page 936 (GBK), so the two do not match and Chinese turns into
#  mojibake.  `chcp 65001` switches THIS window to UTF-8 before the exe
#  runs, which fixes it.  Keep this line first in the command string.
# ===================================================================
param(
    [Parameter(Mandatory = $true)][string]$Exe,
    [string]$Arg = '',
    [Parameter(Mandatory = $true)][string]$WorkDir,
    [string]$Title = 'program'
)

$line = 'chcp 65001 >nul & title ' + $Title + ' & call "' + $Exe + '"'
if ($Arg -ne '') { $line += ' "' + $Arg + '"' }

Start-Process -FilePath 'cmd.exe' -ArgumentList ('/k ' + $line) -WorkingDirectory $WorkDir | Out-Null
