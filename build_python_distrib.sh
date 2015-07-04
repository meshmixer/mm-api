#!/bin/bash

pushd .
cd distrib
rm -rf python_osx
popd

pushd .
cd build
if [[ ! -d "swigosx" ]] ; then
    ./install_swig_osx.sh
fi
./build_python_osx.sh
./make_package_python.sh
rm -rf python_osx
popd
