SET VS=E:\Programas\VStudio
SET PROJ=C:\Users\caioc\Desktop\BRCheats CS\Osiris
CALL "%VS%\Common7\Tools\vcvars32.bat"
cl.exe /EHsc "%PROJ%\program.cpp"
IF NOT %ERRORLEVEL%==0 GOTO Done
:Done