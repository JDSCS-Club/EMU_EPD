@echo off

REM ------------------------------------------------------------------------------------
REM -- _Make.bat V1.3
REM --
REM -- Auto Module Make.
REM -- _Make.bat [_Make.bat 상대경로]
REM -- ex) _Make.bat ..\ [Debug/Release/Boot_Rel] [BottomBd/Bootloader]
REM ------------------------------------------------------------------------------------

REM -- 설정정보 얻기.
rem call _Config.bat

REM		인자가 있는경우 Skip
if "%2" == "" (set CONFIG=Release) else (set CONFIG=%2)

if "%3" == "" (set MODULE=BottomBd) else (set MODULE=%3)

REM -- 날짜 자동으로 넣어주기.
for /F "tokens=1" %%a in ('date /t') do set DATE_TODAY=%%a

set REV_FILE=ReleaseNote_%MODULE%.txt
REM -- svn log -r HEAD:0 > bin\%REV_FILE%
rem git log -r HEAD:0 > bin\%REV_FILE%
git log --pretty=format:"%%ad %%cn %%d  %%s" --graph --all .  > %REV_FILE%

REM -- FileVersion 얻기.
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

REM ------------------------------------------------------
REM		Bootloader + Application
REM if A != B => [ if A neq B ] or [ if not A == B ]
if "%MODULE%" neq "Bootloader" (goto GEN_BOOTAPP) else (goto SKIP_BOOTAPP)
:GEN_BOOTAPP

set IMG_NAME=RFMBase_%TODAY%_v1.%VER%

echo gen "bin\Boot%IMG_NAME%.hex"

REM	#	Del last line. ( .hex file )
findstr /V ":00000001FF" "%1\Boot_Rel\Bootloader.hex" > "%1\..\bin\Boot%IMG_NAME%.hex"

REM	#	Append app.hex
type "%1\%2\%MODULE%.hex" >> "%1\..\bin\Boot%IMG_NAME%.hex"

REM	#	Copy Bin / Hex File
set BIN_FILE_NAME=%IMG_NAME%.bin
set HEX_FILE_NAME=%IMG_NAME%.hex

echo copy "%CONFIG%\%MODULE%.bin" "bin\%BIN_FILE_NAME%"
copy "%1\%CONFIG%\%MODULE%.bin" "%1\..\bin\%BIN_FILE_NAME%"

echo copy "%CONFIG%\%MODULE%.hex" "bin\%HEX_FILE_NAME%"
copy "%1\%CONFIG%\%MODULE%.hex" "%1\..\bin\%HEX_FILE_NAME%"

:SKIP_BOOTAPP
REM ------------------------------------------------------

REM ------------------------------------------------------
REM		인자가 있는경우 Skip
if "%1" == "" (goto PAUSE) else (goto SKIP)
:PAUSE
pause
:SKIP
REM ------------------------------------------------------
