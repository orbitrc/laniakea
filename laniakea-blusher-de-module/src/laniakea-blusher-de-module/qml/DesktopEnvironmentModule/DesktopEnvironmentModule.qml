import QtQuick 2.0

import Blusher.DesktopEnvironment 0.1

import DesktopEnvironmentModule 0.1

Item {
  id: root

  property var menuDelegate: null
  property var menuItemDelegate: null

  property int menuBarHeight: 30

  property int pixelsPerDp: 1

  function onAppCursorChanged(cursor) {
    DesktopEnvironment.standaloneDeModule.onAppCursorChanged(cursor);
  }

  function onMenuOpened(parent, menu) {
    print(['Laniakea::DesktopEnvironmentModule.onMenuOpened']);
    DeModule.openMenu(menu);
//    DesktopEnvironment.standaloneDeModule.onMenuOpened(parent, menu);
  }

  function onMenuClosed() {
    print(['Laniakea::DesktopEnvironmentModule.onMenuClosed']);
//    DesktopEnvironment.standaloneDeModule.onMenuClosed();
  }

  function shortcutToString(shortcut) {
    return DesktopEnvironment.standaloneDeModule.shortcutToString(shortcut);
  }

  Component.onCompleted: {
    print('[DesktopEnvironmentModule.onCompleted] ' + DeModule.hour);
  }
}
