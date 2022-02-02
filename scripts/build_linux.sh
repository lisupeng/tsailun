export set CXXFLAGS=-DDEV_EDITION
export set TSLVERSION=0.0.0


# 1. build frontend
cd ../frontend
yarn
yarn build
cd ../scripts

# 2. build backend
cd ../backend
mkdir build
cd build
cmake ..
make
cd ../../scripts

# 3. make package
cd ..
rm -rf output
mkdir output
cd output
mkdir -p release_linux64/bin
mkdir -p release_linux64/server
cd ../scripts

#copy  fontend build output
cp -rf ../frontend/build/* ../output/release_linux64/server

#copy files to bin
cp ../backend/build/TslServer ../output/release_linux64/bin/
cp ../backend/build/CPPWebFramework/libCPPWebFramework.so ../output/release_linux64/bin/
cp ../backend/build/xdiff/libxdiff.so ../output/release_linux64/bin/

#copy qt files
cp /opt/Qt5.6.3/5.6.3/gcc_64/lib/libQt5Network.so.5 ../output/release_linux64/bin/
cp /opt/Qt5.6.3/5.6.3/gcc_64/lib/libQt5Xml.so.5 ../output/release_linux64/bin/
cp /opt/Qt5.6.3/5.6.3/gcc_64/lib/libQt5Sql.so.5 ../output/release_linux64/bin/
cp /opt/Qt5.6.3/5.6.3/gcc_64/lib/libQt5Core.so.5 ../output/release_linux64/bin/

cp -rf /opt/Qt5.6.3/5.6.3/gcc_64/plugins/sqldrivers ../output/release_linux64/bin/

cp /opt/Qt5.6.3/5.6.3/gcc_64/lib/libicui18n.so.56 ../output/release_linux64/bin/
cp /opt/Qt5.6.3/5.6.3/gcc_64/lib/libicuuc.so.56 ../output/release_linux64/bin/
cp /opt/Qt5.6.3/5.6.3/gcc_64/lib/libicudata.so.56 ../output/release_linux64/bin/