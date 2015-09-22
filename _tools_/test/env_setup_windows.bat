@echo off

if "%1" == "x86"       goto x86
if "%1" == "amd64"     goto amd64

set PATH="C:\Program Files (x86)\Windows Kits\10\Debuggers\x64";%PATH%

:x86
@set PATH=c:\work\whisper\bin\windows_vc_x86\client;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86\compiler;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86\custom;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86\dbs;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86\interpreter;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86\server;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86\stdlib;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86\utils;%PATH%
@echo "The path for Windows 32 bit executables is set!"
goto eof

:amd64
@set PATH=c:\work\whisper\bin\windows_vc_x86_64\client;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86_64\compiler;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86_64\custom;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86_64\dbs;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86_64\interpreter;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86_64\server;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86_64\stdlib;%PATH%
@set PATH=c:\work\whisper\bin\windows_vc_x86_64\utils;%PATH%
@echo "The path for Windows 64 bit executables is set"
goto eof

:eof
