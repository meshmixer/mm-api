IF EXIST distrib (
	pushd .
	cd distrib
        rmdir /S /Q csharp_unity
	popd
)

pushd .
cd build
call build_csharp_vs2012_unity.bat
call make_package_csharp_unity.bat
popd
