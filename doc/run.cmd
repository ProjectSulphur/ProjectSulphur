doxygen.exe doxyfile &
xcopy /s /y "./img" "./generated/html"
call "warning-parser/bin/Release/warning-parser.exe" "warnings.txt"