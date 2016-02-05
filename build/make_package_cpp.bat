SET OUTPATH=..\distrib\cpp

rmdir /S /Q %OUTPATH%
mkdir %OUTPATH%

copy StoredCommands.* %OUTPATH%
copy LiveMesh.* %OUTPATH%
copy lgConfig.h %OUTPATH%
copy stdafx.h %OUTPATH%
copy BinarySerializer.h %OUTPATH%


