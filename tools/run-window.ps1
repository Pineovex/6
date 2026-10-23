# ===================================================================
#  run-window.ps1 --- open a program in its OWN black console window
#  called by tools\run.cmd  (run.cmd /w ...)
#
#  Why not "start"?  Because a child of `start` inherits our
#  stdout/stderr handles, so the VS Code task would stay "running"
#  until the program exits.  Start-Process detaches completely, so the
#  task finishes the moment the window opens.
# ===================================================================
param(
    [Parameter(Mandatory = $true)][string]$Exe,
    [string]$Arg = '',
    [Parameter(Mandatory = $true)][string]$WorkDir,
    [string]$Title = 'program'
)

$line = 'title ' + $Title + ' & call "' + $Exe + '"'
if ($Arg -ne '') { $line += ' "' + $Arg + '"' }

Start-Process -FilePath 'cmd.exe' -ArgumentList ('/k ' + $line) -WorkingDirectory $WorkDir | Out-Null
