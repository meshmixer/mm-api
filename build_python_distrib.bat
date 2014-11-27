IF EXIST distrib (
	pushd .
	cd distrib
	cd python
	del /F /Q *.*
	popd
)

pushd .
cd build
call build_python_vs2012.bat
call make_package_python.bat
popd
