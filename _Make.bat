@echo off

REM ------------------------------------------------------------------------------------
REM -- _Make.bat V1.3
REM --
REM -- Auto Module Make.
REM -- _Make.bat [_Make.bat 상대경로]
REM -- ex) _Make.bat ..\
REM ------------------------------------------------------------------------------------

REM -- 설정정보 얻기.
rem call _Config.bat

REM		인자가 있는경우 Skip
if "%2" == "" (set MODULE=naranja_boron) else (set MODULE=%2)

REM -- 날짜 자동으로 넣어주기.
for /F "tokens=1" %%a in ('date /t') do set DATE_TODAY=%%a

set REV_FILE=ReleaseNote_%MODULE%.txt
REM -- svn log -r HEAD:0 > bin\%REV_FILE%
rem git log -r HEAD:0 > bin\%REV_FILE%
git log --pretty=format:"%%ad %%cn %%d  %%s" --graph --all .  > %REV_FILE%

REM -- FileVersion 얻기.
rem for /f "tokens=5 delims= " %%v in ('filever %EXE_FILE% /b') do set VER=%%v
rem call %1_getVer.bat %1

set shell_cmd=git rev-list HEAD --count .
FOR /F "tokens=*" %%F IN ('%shell_cmd%') DO (SET VER=%%F)

echo %VER%

REM echo %VER%

set yy=%DATE_TODAY:~2,2%
set mm=%DATE_TODAY:~5,2%
set dd=%DATE_TODAY:~8,2%
set TODAY=%yy%%mm%%dd%

set BIN_FILE_NAME=%MODULE%_%TODAY%_v1.%VER%.bin
set HEX_FILE_NAME=%MODULE%_%TODAY%_v1.%VER%.hex
REM set BOOT_FILE_NAME=smrt7tni_bootloader_%TODAY%_v%VER%.hex

echo copy "Release\%MODULE%.bin" "bin\%BIN_FILE_NAME%"
copy "%1Release\%MODULE%.bin" "%1bin\%BIN_FILE_NAME%"

echo copy "Release\%MODULE%.hex" "bin\%HEX_FILE_NAME%"
copy "%1Release\%MODULE%.hex" "%1bin\%HEX_FILE_NAME%"

rem echo copy "MDK-ARM\smrt7tni_bootloader\smrt7tni_bootloader.hex" "bin\%BOOT_FILE_NAME%"
rem copy ".\MDK-ARM\smrt7tni_bootloader\smrt7tni_bootloader.hex" ".\bin\%BOOT_FILE_NAME%"

REM ------------------------------------------------------
REM		인자가 있는경우 Skip
if "%1" == "" (goto PAUSE) else (goto SKIP)
:PAUSE
pause
:SKIP
REM ------------------------------------------------------
