# utf8-terminal.ps1
# 由 VSCode 终端 profile「PowerShell UTF-8」以 -File 方式调用。
# 作用：把当前控制台代码页切到 UTF-8，并让 PowerShell 的输入/输出编码也用 UTF-8，
#       这样 gcc/g++/python 程序输出的中文不会再变成乱码。
# 为什么单独写成脚本：直接在 settings.json 的 args 里写带分号的 -Command，
# 会被 VSCode 拼成畸形命令行，PowerShell 报 ParserError(UnexpectedToken) 起不来终端。

try {
    chcp 65001 > $null
} catch {
}

try {
    [Console]::OutputEncoding = [System.Text.Encoding]::UTF8
    [Console]::InputEncoding  = [System.Text.Encoding]::UTF8
    $OutputEncoding           = [System.Text.Encoding]::UTF8
} catch {
}

# ---------------------------------------------------------------------------
# VS Code starts a task in this terminal by spawning:
#   powershell -NoProfile -NoExit -ExecutionPolicy Bypass -File <this script> -Command <payload...>
# Because of "-File", PowerShell hands "-Command <payload>" to this script as
# $args instead of executing it.  Consequence: the task command never ran, the
# shell just sat idle (thanks to -NoExit), so VS Code kept the task in the
# "running" state forever (status bar stuck on "Building").  Every later F5 /
# preLaunchTask then queued behind that busy shared terminal and looked dead.
# Fix: run the payload ourselves.  When it came from a task (not from shell
# integration) exit afterwards, so VS Code sees the task finish properly.
# ---------------------------------------------------------------------------
if ($args.Count -ge 2 -and $args[0] -eq '-Command') {
    $payload = ($args[1..($args.Count - 1)] -join ' ')
    $isShellIntegration = $payload -match 'shellIntegration'
    try {
        Invoke-Expression $payload
    } catch {
        Write-Host ("[utf8-terminal] failed to run -Command payload: " + $_.Exception.Message)
    }
    if (-not $isShellIntegration) {
        $exitCode = $LASTEXITCODE
        if ($null -eq $exitCode) { $exitCode = 0 }
        exit $exitCode
    }
}
