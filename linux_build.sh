if [ ! -d build ];then
    mkdir build
else
    rm -rf build/*
fi

cd build
qmake ../hexwalk/hexwalk.pro
make
cd ..
