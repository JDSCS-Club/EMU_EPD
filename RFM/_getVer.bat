@echo off

REM ------------------------------------------------------------------------------------
REM -- _getVer.bat V1.0
rem	--	src/version.h에서 버전정보 추출 스크립트.
REM -- _getVer.bat [상대경로]
REM -- ex) _getVer.bat ..\
REM ------------------------------------------------------------------------------------

set VER_FILE=%1Library\version.h

%windir%\system32\FIND "#define	APP_MAIN_VER" %VER_FILE%	> %temp%\TEMPVER.txt
FOR /F "tokens=3 delims=	" %%a in (%temp%\TEMPVER.txt) do set ver_main=%%a
%windir%\system32\FIND "#define	APP_SUB_VER" %VER_FILE%	> %temp%\TEMPVER.txt
FOR /F "tokens=3 delims=	" %%a in (%temp%\TEMPVER.txt) do set ver_sub=%%a
%windir%\system32\FIND "#define	APP_DETAIL_VER" %VER_FILE%	> %temp%\TEMPVER.txt
FOR /F "tokens=3 delims=	" %%a in (%temp%\TEMPVER.txt) do set ver_det=%%a
%windir%\system32\FIND "#define	APP_BUILD_VER" %VER_FILE%	> %temp%\TEMPVER.txt
FOR /F "tokens=3 delims=	" %%a in (%temp%\TEMPVER.txt) do set ver_build=%%a

set	VER=%ver_main%.%ver_sub%.%ver_det%.%ver_build%

echo VER : %VER%
