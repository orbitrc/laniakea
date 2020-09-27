
default:
	rm -rf build
	mkdir build
	cd build && qmake ../laniakea-shell.pro -spec linux-g++ && make qmake_all
	cd build && make -j8

install: build/laniakea-shell
	cp build/laniakea-shell /usr/bin/laniakea-shell
