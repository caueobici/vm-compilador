main:
	g++ ./src/* -o ./vm.so -shared -fPIC
	g++ ./src/* -o ./vm

windows:
	x86_64-w64-mingw32-g++ ./src/* -o ./vm.exe -static
	x86_64-w64-mingw32-g++ ./src/* -o ./vm.dll -static -shared