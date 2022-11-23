@echo off
cd %~dp0
mkdir build
cd build
cmake ..
cmake --build . --config release
cd ..
copy build\Release\TSSP.* . /Y
del /s /q build >NUL
rmdir /s /q build >NUL