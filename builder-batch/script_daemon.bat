@echo off
set /p INPUTFOLDER="Please enter which folder you want to process (e.g. ../dax-jakster/): "
echo %INPUTFOLDER%*.lua

:loop
timeout -t 1 >nul 
for /R %INPUTFOLDER%unprocessed/ %%f in (*.lua) do call :check_file %%f
goto :loop

:check_file
	set file=%1
	for %%i in (%file%) do echo %%~ai|find "a">nul || goto :eof
	call :convert_script %file%	
	attrib -a %file%
goto :eof

:convert_script
set file=%1
echo %file% Changed repackaging scripts 
sulphur-builder.exe --convert_scripts -r -dir %INPUTFOLDER%unprocessed/ -output %INPUTFOLDER%processed/
goto :eof