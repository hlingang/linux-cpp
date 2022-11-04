rm -rf bin build python libs
mkdir build
cd build
cmake -G"Unix Makefiles" ..
make
cd ..

if [ -f libs/_ctestlib.so ]
then 
    cp libs/_ctestlib.so python/
fi