@echo off

set BUILD_DIR=build

if not exist %BUILD_DIR% (
  mkdir %BUILD_DIR%
)

cd %BUILD_DIR%
cmake .. -G"Visual Studio 14 2015 Win64" -DCHECK_MEMORY_LEAKS=1
cmake --build .
 
