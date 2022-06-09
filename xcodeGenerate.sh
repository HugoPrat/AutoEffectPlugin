#Should have a libtorch in the root project folder (same folder as CMakeList.txt)
cmake -DCMAKE_PREFIX_PATH=./libtorch -B Builds -G Xcode