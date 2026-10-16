@ECHO off
ECHO processing....
 
::note 1
FOR /R  %%i IN (*.h) DO (                                                            
::note 2
    IF EXIST %%i (ECHO %%~pi >> tmp)                                                
)
 
FOR /F "delims=" %%i IN (tmp) DO ( 
IF NOT DEFINED %%i SET %%i=A & ECHO %%i>>HeadDirFile.txt) 
 
DEL tmp
pause