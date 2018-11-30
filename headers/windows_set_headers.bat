ECHO Fixing the git header links ...

FOR %%D IN (dbs client compiler interpreter server stdlib utils extutils) DO (
    DEL /AS /F %%D
    IF EXIST %%D RMDIR /AS /F %%D
    MKLINK /D %%D ..\%%D\include
)

echo You are ready to rock ... Hit it!
    
