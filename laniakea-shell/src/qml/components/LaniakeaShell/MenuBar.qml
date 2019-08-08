import QtQuick 2.12
import QtQuick.Window 2.12 as QtQuickWindow

import Blusher 0.1
import Blusher.DesktopEnvironment 0.1

import LaniakeaShell 0.1
import Laniakea 0.1

QtQuickWindow.Window {
  id: root
  flags: Qt.Popup
  width: QtQuickWindow.Screen.width
  height: 30

  color: "#181818"
  property var systemMenu
  property var applicationMenu: Menu {
    title: 'Application'
    type: Menu.MenuType.Submenu
  }
  property var menuBarMenu: Menu {
    title: 'Menu Bar Menu'
    type: Menu.MenuType.MenuBarMenu
  }

  property list<Menu> menus
  property string clock: ''
  property Menu testMenu: Menu {
    type: Menu.MenuType.MenuBarMenu
    title: 'TestMenu'
    MenuItem {
      title: 'Item 1'
    }
    MenuItem {
      title: 'Item 2'
    }
  }


  Row {
    anchors.fill: parent
    // System menu
    Rectangle {
      id: menuBarSystemMenu
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      width: root.height
      color: "grey"

      PopUpMenuDelegate {
        id: systemMenuDelegate
        menu: root.systemMenu // still be used?

        path: '/'
        items: root.systemMenu.items
        onItemTriggered: {
          let item = root.menuGetItem(systemMenuDelegate.menu, path);
          if (item !== null && item.action) {
            item.action();
          }
        }
      }
      MouseArea {
        anchors.fill: parent
        onClicked: {
          systemMenuDelegate.show();
        }
      }
    }
    // Application menu
    MenuBarMenuItemDelegate {
      id: menuBarItemApplicationMenu

      title: root.applicationMenu.title
      bold: true

      visible: root.applicationMenu.items.length > 0
      anchors.top: parent.top
      anchors.bottom: parent.bottom

      PopUpMenuDelegate {
        id: applicationMenuDelegate
        menu: root.applicationMenu
        path: '/'
        items: root.applicationMenu.items

        onItemTriggered: {
          Shell.applicationMenuItemTriggered(path);
        }
      }

      onTriggered: {
        print('Application Menu Triggered!');
        applicationMenuDelegate.show();
      }
    }

    // Menu bar menu
    Repeater {
      id: menuItemRepeater
      model: root.menuBarMenu.items.length
      delegate: Rectangle {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: text.implicitWidth
        color: "cyan"
        Text {
          id: text
          text: root.menuBarMenu.items[index].title
          anchors.verticalCenter: parent.verticalCenter
          rightPadding: 7.0 * DesktopEnvironment.pixelsPerDp
          leftPadding: 7.0 * DesktopEnvironment.pixelsPerDp
          font.pixelSize: 14 * DesktopEnvironment.pixelsPerDp
        }
      }
      resources: [
        Connections {
          target: Shell
          onMenuBarMenuChanged: {
            menuItemRepeater.model = Shell.menuBarMenu.items.length;
          }
        }
      ]
    }
  } // Row

  MouseArea {
    width: 32
    x: 150
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    Rectangle {
      anchors.fill: parent
      color: "lightgreen"
    }
    onClicked: {
//      Shell.openMenu(root.testMenu);
    }
  }

  MouseArea {
    width: 32
    x: 300
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    Rectangle {
      anchors.fill: parent
      color: "red"
    }
    onClicked: {
      Shell.quit();
    }
  }

  Row {
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    // Desktops
    Rectangle {
      width: 100
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      color: "blue"
      Repeater {
        model: Shell.numberOfDesktops
        Rectangle {
          x: index * this.width
          anchors.top: parent.top
          anchors.bottom: parent.bottom
          width: (100 / Shell.numberOfDesktops)
          color: (Shell.currentDesktop === index + 1) ? "white" : "red"
          border.width: 1
          Text {
            text: index + 1
          }
        }
      }
    }
    // Clock
    MouseArea {
      width: 100
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      Rectangle {
        anchors.fill: parent
        color: "cyan"
        Label {
          text: root.clock
        }
      }
      onClicked: {
      }
    }
  }

  onMenuBarMenuChanged: {
    print('onMenuBarMenuChanged: ' + root.menuBarMenu);
  }

  Component.onCompleted: {
    root.systemMenu = Menus.systemMenu;
  }
  Component.onDestruction: {
    print('[MenuBar] destruction');
  }

  function menuGetItem(menu, path) {
    for (let i = 0; i < menu.items.length; ++i) {
      if (menu.items[i].path === path) {
        return menu.items[i];
      }
    }
    return null;
  }
}
