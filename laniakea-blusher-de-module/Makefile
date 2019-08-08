VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 0
VERSION = 0.1.0
SHARED_LIB_TARGET_DIR = share/laniakea/qml/DesktopEnvironmentModule

default: share/
	rm -rf build
	mkdir build
	cd build && qmake ../laniakea-blusher-de-module.pro -spec linux-g++ CONFIG+=qtquickcompiler && make qmake_all
	cd build && make -j8
	cp build/liblaniakea-blusher-de-module.so.$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH) $(SHARED_LIB_TARGET_DIR)
	ln -s liblaniakea-blusher-de-module.so.$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH) $(SHARED_LIB_TARGET_DIR)/liblaniakea-blusher-de-module.so.$(VERSION_MAJOR).$(VERSION_MINOR)
	ln -s liblaniakea-blusher-de-module.so.$(VERSION_MAJOR).$(VERSION_MINOR) $(SHARED_LIB_TARGET_DIR)/liblaniakea-blusher-de-module.so.$(VERSION_MAJOR)
	ln -s liblaniakea-blusher-de-module.so.$(VERSION_MAJOR) $(SHARED_LIB_TARGET_DIR)/liblaniakea-blusher-de-module.so

share/:
	mkdir share
	mkdir share/laniakea
	cp -r src/laniakea-blusher-de-module/qml share/laniakea/qml

install:
	rm -rf /usr/share/laniakea
	cp -r share/laniakea /usr/share/laniakea

clean:
	rm -rf build
	rm -rf share
