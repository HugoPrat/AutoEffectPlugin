wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.11.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.11.0+cpu.zip
sudo apt-get install libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libasound2-dev
cmake -B Builds -DCMAKE_PREFIX_PATH=./libtorch