cmake --build build --config Debug --target ALL_BUILD -j 14 --
cd build
ctest -j14 -C Debug -T test --output-on-failure