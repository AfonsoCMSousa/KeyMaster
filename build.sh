# Description: Build script for KeyMaster

cd ./build
cmake ..
make

cp "KeyMaster_server" ../bin
