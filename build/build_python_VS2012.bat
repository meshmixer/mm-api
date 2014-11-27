
REM cleanup python output directory
mkdir python
pushd .
cd python
del /F /Q *.*
popd

copy *.py python

REM run swig to generate python 
swigwin\swig -c++ -python -o ./python/mmapi.cpp -outdir python mmAPI.i

pushd .
cd python
REM 

REM [RMS] argh python seems to require specific versions of MSVC++, so the following
REM   hack is actually a bad one. However, it is working so far. See details in answers here:
REM     http://stackoverflow.com/questions/2817869/error-unable-to-find-vcvarsall-bat
REM   It seems like the correct solution is to install VC++ 2008 Express

SET VS90COMNTOOLS=%VS110COMNTOOLS%

REM build mmapi module
python setup.py build_ext --inplace

REM delete build directory
rmdir /S /Q build

popd

