import QtQuick 2.12
import QtQuick.Window 2.12 as QtQuickWindow

import Blusher 0.1
import Blusher.DesktopEnvironment 0.1

import LaniakeaShell 0.1
import Laniakea 0.1

Item {
  id: root

  property var systemMenu: null

  property string clock: ''

  width: 0
  height: 0

  MenuBar {
    id: _test
    visible: true

    clock: root.clock
  }


  Timer {
    interval: 1000
    running: true
    repeat: true
    onTriggered: {
      root.clock = new Date();
    }
  }

  Connections {
    target: Shell
    onRegisterMenuBarMenu: {
      let menu = JSON.parse(menuJson);
      Shell.menuBarMenu = menu;
    }
  }

  Component.onCompleted: {
    root.systemMenu = Menus.systemMenu;
    root.clock = new Date();
  }

  Component.onDestruction: {
    print('main destruction');
  }
}
