export CXX=/usr/bin/g++
cmake . -B build_linux -DCMAKE_BUILD_TYPE=Debug
make -C build_linux
./build_linux/clay_test
