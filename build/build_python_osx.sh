#!/bin/bash

PYDIR=python_osx

# cleanup python output directory
rm -rf $PYDIR
mkdir $PYDIR

# run swig to generate python 
swigosx/bin/swig -c++ -python -o ./$PYDIR/mmapi.cpp -outdir $PYDIR mmAPI.i

# copy other files
cp *.h $PYDIR
cp lgConfig_osx.h $PYDIR/lgConfig.h
cp *.cpp $PYDIR
cp *.py $PYDIR
pushd .
cd $PYDIR
perl -i -pe 's/\<lgConfig\.h\>/\"lgConfig\.h\"/' BinarySerializer.h
perl -i -pe 's/\<BinarySerializer\.h\>/\"BinarySerializer\.h\"/' StoredCommands.h
popd


pushd .
cd $PYDIR

# build mmapi module
python setup.py build_ext --inplace

# delete build directory
rm -rf build

popd

