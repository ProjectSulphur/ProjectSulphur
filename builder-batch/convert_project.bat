@echo off
echo PLEASE READ THE README FIRST! FIND IT IN THE SAME FOLDER AS THIS BATCH SCRIPT!
set /p INPUTFOLDER="Please enter which folder you want to process (e.g. ../dax-jakster/): "
set /p DEFAULTVERTEX="Please enter the NAME of the default vertex shader to use (as defined in your input folder): "
set /p DEFAULTPIXEL="Please enter the NAME of the default pixel shader to use (as defined in your input folder): "
call .\batch-files\clean.bat
call .\batch-files\convert_shaders.bat
call .\batch-files\convert_models.bat
call .\batch-files\convert_animations.bat
call .\batch-files\convert_skeletons.bat
call .\batch-files\convert_textures.bat
call .\batch-files\convert_scripts.bat
call .\batch-files\convert_audio.bat
rd /s /q "./sulphur-builder-output"
del ".\*.cache"
pause