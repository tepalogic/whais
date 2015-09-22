@echo off

if "%1" == "x86"       goto x86
if "%1" == "amd64"     goto amd64

:x86
rmdir bin
MKLINK /D bin whisper\bin\windows_vc_x86

@echo The path for Windows 32 bit executables is set!
goto eof

:amd64
rmdir bin
MKLINK /D bin whisper\bin\windows_vc_x86_64

@echo The path for Windows 64 bit executables is set
goto eof

:eof
