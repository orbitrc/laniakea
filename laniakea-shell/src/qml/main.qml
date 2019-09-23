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
    id: menuBar
    visible: true

    systemMenu: root.systemMenu
    clock: root.clock
  }


  Timer {
    interval: 1000
    running: true
    repeat: true
    onTriggered: {
//      root.clock = new Date();
      let now = new Date();
      let text = '';
      let dateString = {
        hour: now.getHours().toString().padStart(2, '0'),
        minute: now.getMinutes().toString().padStart(2, '0'),
        second: now.getSeconds().toString().padStart(2, '0')
      };

      text += dateString.hour + ':' + dateString.minute + ':' + dateString.second;
      root.clock = text;
    }
  }

  SystemPreferences {
    id: systemPreferences
    visible: false
  }

  ApplicationLauncher {
    id: applicationLauncher
    visible: false
  }

  Connections {
    target: Shell
    onRegisterApplicationMenu: {
      let menuJsonObject = JSON.parse(menuJson);

      menuBar.applicationMenu.addItem();
//      menuBar.applicationMenu.addItem();
    }

    onRegisterMenuBarMenu: {
      let menu = JSON.parse(menuJson);
      Shell.menuBarMenu = menu;
    }

    onApplicationMenuRegisterRequested: {
      let menu = JSON.parse(menuJson);
      menuBar.applicationMenu.title = menu.title;
      menuBar.applicationMenu.items = menu.items;
    }

    onRunCommandPopUpOpenRequested: {
      applicationLauncher.show();
    }
  }

  Component.onCompleted: {
    menuBar.show();

    root.systemMenu = Menus.systemMenu;
    Shell.systemMenu = Menus.systemMenu;
    Shell.systemPreferences = systemPreferences;
    root.clock = new Date();
  }

  Component.onDestruction: {
    print('main destruction');
  }
}
