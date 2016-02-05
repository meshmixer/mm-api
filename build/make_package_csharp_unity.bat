SET OUTPATH=..\distrib\csharp_win_unity

rmdir /S /Q %OUTPATH%
mkdir %OUTPATH%

mkdir %OUTPATH%\Release
copy csharp_vs\bin\Release\*.* %OUTPATH%\Release

mkdir %OUTPATH%\Debug
copy csharp_vs\bin\Debug\*.* %OUTPATH%\Debug

copy StoredCommands.h %OUTPATH%\Release
copy StoredCommands.h %OUTPATH%\Debug
copy csharp_vs\RemoteControl.cs %OUTPATH%\Release
copy csharp_vs\RemoteControl.cs %OUTPATH%\Debug

