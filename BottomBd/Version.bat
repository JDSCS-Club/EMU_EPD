@echo off

rem "${ProjDirPath}\Version.bat" "${ProjDirPath}" "${ProjDirPath}\Library\version.h.template" "${ProjDirPath}\Library\version.h"
echo %1 %2 %3
echo START Process
rem git rev-list HEAD --count > ver.txt
rem set /p WCREV=<ver.txt
rem del ver.txt
set shell_cmd=git rev-list HEAD --count %1
FOR /F "tokens=*" %%F IN ('%shell_cmd%') DO (SET WCREV=%%F)

echo %WCREV%

rem powershell -Command "(gc %1) -replace '\$WCREV\$', %WCREV% | Out-File %2"
rem powershell -Command "(gc %1) -replace '\$WCREV\$', %WCREV% | Out-File %2 -Encoding UTF8"

rem UTF-8(BOM없음)으로 출력.
powershell -Command "$Stream = (Get-Content -Encoding UTF8 %2) -replace '\$WCREV\$', %WCREV%; $Utf8NoBomEncoding = New-Object System.Text.UTF8Encoding $False; [System.IO.File]::WriteAllLines(\"%3\", $Stream, $Utf8NoBomEncoding)"

echo END Process

