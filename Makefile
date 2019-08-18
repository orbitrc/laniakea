install:
	rm -rf /usr/share/laniakea
	cp -r share/laniakea /usr/share/laniakea
	cp share/xsessions/* /usr/share/xsessions/
	cp share/wayland-sessions/* /usr/share/wayland-sessions/
