cd ../backend
rm -rf build_nmake_dev
mkdir build_nmake_dev
cd build_nmake_dev
set CXXFLAGS=-DDEV_EDITION
cmake -DCMAKE_BUILD_TYPE=Release .. -G"NMake Makefiles"
nmake
cd ../../scripts