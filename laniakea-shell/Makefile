
default:
	rm -rf build
	mkdir build
	cd build && qmake ../laniakea-shell.pro -spec linux-g++ CONFIG+=qtcuickcompiler && make qmake_all
	cd build && make -j8
