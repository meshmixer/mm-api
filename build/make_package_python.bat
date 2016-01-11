SET OUTPATH=..\distrib\python

rmdir /S /Q %OUTPATH%
mkdir %OUTPATH%

copy python\_mmapi.pyd %OUTPATH%
copy python\setup.py %OUTPATH%
copy python\mmapi.py %OUTPATH%
copy python\mmRemote.py %OUTPATH%
copy python\test.py %OUTPATH%

copy StoredCommands.h %OUTPATH%

copy LiveMesh.* %OUTPATH%


