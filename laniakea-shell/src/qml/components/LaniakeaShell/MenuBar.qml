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
      fixedWidth: 40

      menuItems: root.systemMenu.items

      onMenuItemTriggered: {
        let item = root.getMenuItemByPath(this.menuItems, path);
        if (item !== null && item.action) {
          item.action();
        }
      }

      onPopUpOpened: {
        root.focusedMenuItemIndex = 0;
      }
      onPopUpClosed: {
        root.focusedMenuItemIndex = -1;
      }
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
    id: menuBarExtensions

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    // Power
    MenuBarItemDelegate {
      itemType: MenuBarItemDelegate.ItemType.MenuBarExtension
      visualType: MenuBarItemDelegate.VisualType.TitleOnly
      title: Shell.batteryLevel + '%' + (Shell.charging ? '*' : '')
//      image: 'qrc:/assets/orbit-logo-light@256x256.png'
      focused: root.focusedExtensionIndex === root.getExtensionIndex(this)
      menuItems: [
        { path: '/asdf', title: 'Asdf' },
        { path: '/fdsa', title: 'Open Power Saving Preference ...' }
      ]

      onPopUpOpened: {
        root.focusedExtensionIndex = root.getExtensionIndex(this);
      }
      onPopUpClosed: {
        root.focusedExtensionIndex = -1;
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

    // Desktops
    MenuBarItemDelegate {
      title: '[ ' + Shell.currentDesktop + ' ]'

      anchors.top: parent.top
      anchors.bottom: parent.bottom

      menuItems: []
      onMenuItemTriggered: {
        let item = root.getMenuItemByPath(this.menuItems, path);
        if (item !== null && item.action) {
          item.action();
        }
      }
      onPopUpAboutToOpen: {
        this.menuItems = createItems();
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

  // deprecated
  function menuGetItem(menu, path) {
    return root.getMenuItemByPath(menu.items, path);
  }

  function getMenuItemByPath(items, path) {
    for (let i = 0; i < items.length; ++i) {
      if (items[i].path === path) {
        return items[i];
      }
    }
    return null;
  }

  function getExtensionIndex(extension) {
    for (let i = 0; i < menuBarExtensions.children.length; ++i) {
      if (extension === menuBarExtensions.children[i]) {
        return i;
      }
    }
    return -1;
  }
}
