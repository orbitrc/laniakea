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
  property Menu systemMenu: null
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

  MenuBarMenuDelegate {
    id: menuBarMenuDelegate

    menuBar: root
    width: 100
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    Row {
      anchors.fill: parent
      Rectangle {
        id: menuBarSystemMenu
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 30 * DesktopEnvironment.pixelsPerDp
        color: "grey"
      }
      Repeater {
        id: menuItemRepeater
        model: 0
        Rectangle {
          anchors.top: parent.top
          anchors.bottom: parent.bottom
          width: text.implicitWidth
          color: "cyan"
          Text {
            id: text
            text: Shell.menuBarMenu.items[index].title
            anchors.verticalCenter: parent.verticalCenter
            rightPadding: 7.0 * DesktopEnvironment.pixelsPerDp
            leftPadding: 7.0 * DesktopEnvironment.pixelsPerDp
            font.pixelSize: 14 * DesktopEnvironment.pixelsPerDp
          }
        }
      }
    } // Row
    Component.onDestruction: {
      print('menubarmenudelegate destruction');
    }
  }

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
      Shell.openMenu(root.testMenu);
//      Shell.openMenu();
    }
  }

  MouseArea {
    width: 32
    x: 200
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

  MouseArea {
    width: 100
    anchors.right: parent.right
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

  Connections {
    target: Shell
    onMenuBarMenuChanged: {
      menuItemRepeater.model = Shell.menuBarMenu.items.length;
    }
  }

  Component.onCompleted: {
    root.systemMenu = Menus.systemMenu;
  }
  Component.onDestruction: {
    print('[MenuBar] destruction');
  }
}
