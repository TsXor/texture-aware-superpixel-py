@echo off
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
REM set default generator to mingw permanently because people who use msvc
REM should have been using its cmake instead of installing it from chocolatey
REM 永久设定默认生成器为mingw，毕竟用msvc的都用自带的cmake了,
REM 根本不用通过chocolatey再装一个
setx CMAKE_GENERATOR "MinGW Makefiles" /m