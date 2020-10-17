Laniakea Desktop Environment
=============================

Installed files
-------------------

```
/
|
+- usr
   |
   +- bin
   |  |- laniakea-shell
   |  |- laniakea-x11 (xsession executable)
   |  +- laniakea-wayland (wayland session executable. DO NOT USE THIS YET)
   |
   +- share
      |
      +- laniakea
      |  |
      |  +- qml
      |  |  |
      |  |  +- DesktopEnvironmentModule/  (Blusher DE module path)
      |  |
      |  +- wallpapers (default wallpapers)
      |
      |- xsessions/laniakea.desktop  (SDDM xsession file)
      +- wayland-sessions/laniakea.desktop
```

Install Session
----------------
```sh
$ make install
```
