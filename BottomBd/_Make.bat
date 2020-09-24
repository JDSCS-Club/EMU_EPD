@echo off

REM ------------------------------------------------------------------------------------
REM -- _Make.bat V1.3
REM --
REM -- Auto Module Make.
REM -- _Make.bat [_Make.bat �����]
REM -- ex) _Make.bat ..\ [Debug/Release/Boot] [RFM/Bootloader]
REM ------------------------------------------------------------------------------------

REM -- �������� ���.
rem call _Config.bat

REM		���ڰ� �ִ°�� Skip
if "%2" == "" (set CONFIG=Release) else (set CONFIG=%2)

if "%3" == "" (set MODULE=BottomBd) else (set MODULE=%3)

REM -- ��¥ �ڵ����� �־��ֱ�.
for /F "tokens=1" %%a in ('date /t') do set DATE_TODAY=%%a

set REV_FILE=ReleaseNote_%MODULE%.txt
REM -- svn log -r HEAD:0 > bin\%REV_FILE%
rem git log -r HEAD:0 > bin\%REV_FILE%
git log --pretty=format:"%%ad %%cn %%d  %%s" --graph --all .  > %REV_FILE%

REM -- FileVersion ���.
rem for /f "tokens=5 delims= " %%v in ('filever %EXE_FILE% /b') do set VER=%%v
rem call %1_getVer.bat %1

set shell_cmd=git rev-list HEAD --count %1
FOR /F "tokens=*" %%F IN ('%shell_cmd%') DO (SET VER=%%F)

echo %VER%

REM echo %VER%

set yy=%DATE_TODAY:~2,2%
set mm=%DATE_TODAY:~5,2%
set dd=%DATE_TODAY:~8,2%
set TODAY=%yy%%mm%%dd%

REM set BIN_FILE_NAME=%MODULE%_%TODAY%_v1.%VER%.bin
REM set HEX_FILE_NAME=%MODULE%_%TODAY%_v1.%VER%.hex
set BIN_FILE_NAME=RFMBase_%TODAY%_v1.%VER%.bin
set HEX_FILE_NAME=RFMBase_%TODAY%_v1.%VER%.hex
REM set BOOT_FILE_NAME=smrt7tni_bootloader_%TODAY%_v%VER%.hex

echo copy "%CONFIG%\%MODULE%.bin" "bin\%BIN_FILE_NAME%"
copy "%1\%CONFIG%\%MODULE%.bin" "%1\..\bin\%BIN_FILE_NAME%"

echo copy "Release\%MODULE%.hex" "bin\%HEX_FILE_NAME%"
copy "%1\%CONFIG%\%MODULE%.hex" "%1\..\bin\%HEX_FILE_NAME%"

rem echo copy "MDK-ARM\smrt7tni_bootloader\smrt7tni_bootloader.hex" "bin\%BOOT_FILE_NAME%"
rem copy ".\MDK-ARM\smrt7tni_bootloader\smrt7tni_bootloader.hex" ".\bin\%BOOT_FILE_NAME%"

REM ------------------------------------------------------
REM		���ڰ� �ִ°�� Skip
if "%1" == "" (goto PAUSE) else (goto SKIP)
:PAUSE
pause
:SKIP
REM ------------------------------------------------------
