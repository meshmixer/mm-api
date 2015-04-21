#!/bin/bash

# [RMS] hardcoded for pcre 8.35 and swig 3.0.5

# nuke any exisiting swigosx folder
rm -rf swigosx

# download and extract swig 3.0.5, and rename folder to swigosx
curl -L -O http://sourceforge.net/projects/swig/files/swig/swig-3.0.5/swig-3.0.5.tar.gz
tar -xvzf swig-3.0.5.tar.gz
rm swig-3.0.5.tar.gz
mv swig-3.0.5 swigosx

# put PCRE inside the swigosx folder (simplifies cleanup)
pushd .
cd swigosx

# download and extract PCRE
curl --remote-name ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-8.35.tar.gz
tar -xvzf pcre-8.35.tar.gz

# build PCRE
cd pcre-8.35
./configure --prefix=$PWD
make
make install

#add PCRE to path so that swig can find it
export PATH=$PWD/bin:$PATH

# go back to top-level build dir
popd


# build swig
pushd .
cd swigosx
./configure --prefix=$PWD
make
make install
popd
