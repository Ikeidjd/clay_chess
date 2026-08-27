cmake . -B build
if %errorlevel% neq 0 exit /b %errorlevel%
ninja -C build
if %errorlevel% neq 0 exit /b %errorlevel%
.\build\clay_test.exe
