@echo off
cd %~dp0
mkdir build
cd build
cmake ..
cmake --build . --config release
cd ..
copy build\Release\*TSSP.* . /Y >NUL
copy build\*TSSP.* . /Y >NUL
del /s /q build >NUL
rmdir /s /q build >NUL