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

  property int focusedMenuItemIndex: -1
  property int focusedExtensionIndex: -1

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

  Row {
    anchors.fill: parent
    //================
    // System menu
    //================
    MenuBarItemDelegate {
      id: menuBarSystemMenu

      itemType: MenuBarItemDelegate.ItemType.MenuItem
      visualType: MenuBarItemDelegate.VisualType.ImageOnly

      anchors.top: parent.top
      anchors.bottom: parent.bottom
      image: 'qrc:/assets/orbit-logo-light@256x256.png'
      focused: focusedMenuItemIndex === 0

//      PopUpMenuDelegate {
//        menu: root.systemMenu // still be used?

//        path: '/'
      menuItems: root.systemMenu.items
//        onItemTriggered: {
//          let item = root.menuGetItem(systemMenuDelegate.menu, path);
//          if (item !== null && item.action) {
//            item.action();
//          }
//        }
//        onClosed: {
//          root.focusedMenuItemIndex = -1;
//        }
    }
    //===================
    // Application menu
    //===================
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
        applicationMenuDelegate.show(30, 30);
      }
    }
    //================
    // Menu bar menu
    //================
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


  // Kill button
  MouseArea {
    width: 24
    x: 300
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    hoverEnabled: true
    Rectangle {
      id: _testRect
      anchors.fill: parent
      color: "red"
    }
    onClicked: {
      Shell.quit();
    }
    onEntered: {
      _testRect.color = "blue";
    }
    onExited: {
      _testRect.color = "red";
    }
  }

  //==========================
  // Menu bar extensions
  //==========================
  Row {
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    // Power
    MenuBarItemDelegate {
      itemType: MenuBarItemDelegate.ItemType.MenuBarExtension
      visualType: MenuBarItemDelegate.VisualType.ImageOnly
      title: Shell.batteryLevel + '%' + (Shell.charging ? '*' : '')
//      image: 'qrc:/assets/orbit-logo-light@256x256.png'
//        path: '/'
//        items: [
//          { path: '/asdf', title: 'Asdf' },
//          { path: '/fdsa', title: 'Open power saving preference ...' }
//        ]
    }

    // Desktops
    MenuBarMenuItemDelegate {
      title: Shell.currentDesktop

      anchors.top: parent.top
      anchors.bottom: parent.bottom

      PopUpMenuDelegate {
        id: desktopsMenuBarExtensionDelegate

        menuBarRect.x: root.x
        menuBarRect.y: root.y
        menuBarRect.width: root.width
        menuBarRect.height: root.height

        path: '/'
        items: []
        onItemTriggered: {
          let item = root.menuGetItem(desktopsMenuBarExtensionDelegate.menu, path);
          if (item !== null && item.action) {
            item.action();
          }
        }
        Component.onCompleted: {
          desktopsMenuBarExtensionDelegate.items = createItems();
        }
        function createItems() {
          let li = [];
          for (let i = 0; i < Shell.preferences.desktop.numberOfDesktops; ++i) {
            li.push({
              path: '/',
              title: 'Desktop ' + (i + 1)
            });
          }
          return li;
        }
      }

      onTriggered: {
        desktopsMenuBarExtensionDelegate.show(x, 30);
      }
    }
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
          Label {
            text: index + 1
          }
        }
      }
    }
    // Clock
    MenuBarMenuItemDelegate {
      title: root.clock

      anchors.top: parent.top
      anchors.bottom: parent.bottom

      onTriggered: {
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
