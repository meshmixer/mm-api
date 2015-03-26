
REM cleanup python output directory
rmdir /S /Q csharp_vs
mkdir csharp_vs
pushd .
cd csharp_vs
mkdir mmapi
popd

REM run swig to generate python 
swigwin\swig -c++ -csharp -o ./csharp_vs/mmapi.cpp -outdir csharp_vs/mmapi -dllimport mmapi mmAPI.i

REM copy necessary files for VS project
copy BinarySerializer.* csharp_vs
copy StoredCommands.* csharp_vs
copy stdafx.h csharp_vs
copy lgConfig.h csharp_vs

copy csharp_vsproj_files\*.* csharp_vs

pushd .
cd csharp_vs

call %VS110COMNTOOLS%\vsvars32.bat
msbuild mmapi_csharp.sln /t:rebuild /p:Configuration=Debug
msbuild mmapi_csharp.sln /t:rebuild /p:Configuration=Release

popd
