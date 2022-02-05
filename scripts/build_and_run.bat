
cmake . -B bin/debug -G Ninja -D CMAKE_BUILD_TYPE=Debug
@REM cmake -G Ninja --build out --config Debug --target trove
ninja -C bin/debug -j 20 trove

cmake . -B bin/release -G Ninja -D CMAKE_BUILD_TYPE=Release
@REM cmake -G Ninja --build out --config Debug --target trove
ninja -C bin/release -j 20 trove