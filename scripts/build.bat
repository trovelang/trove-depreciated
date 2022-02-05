
set config=%1

cmake . -B bin/%config% -G Ninja -D CMAKE_BUILD_TYPE=%config% -DCODEGEN_GCC=ON
@REM cmake -G Ninja --build out --config Debug --target trove
ninja -C bin/%config% -j 20 trove