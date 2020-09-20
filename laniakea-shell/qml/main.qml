import QtQuick 2.12
import QtQuick.Window 2.12 as QtQuickWindow

import Blusher 0.1

Window {
  id: root

  visible: true
  netWmWindowType: Window.NetWmWindowType.Dock

  x: 0
  y: 0
  width: QtQuickWindow.Screen.width
  height: 30

  color: "transparent"

  Rectangle {
    id: fillColor

    anchors.fill: parent
    color: "#77ffffff"
  }

  //================
  // System menu
  //================
  View {
    width: 50
    anchors.top: parent.top
    anchors.bottom: parent.bottom

    states: [
      State {
        name: 'active'
        PropertyChanges {
          target: activeColor
          color: "blue"
        }
      }
    ]
    Rectangle {
      id: activeColor
      width: 30
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      anchors.right: parent.right
      color: "transparent"
    }
    Image {
      source: 'qrc:/assets/orbit-logo-light@256x256.png'
      anchors.centerIn: parent
      width: 30
      height: 30
    }
    MouseArea {
      anchors.fill: parent

      onClicked: {
        systemMenu.open(0, root.height);
        parent.state = 'active';
      }
    }
  }

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

  //==================
  // Menus
  //==================
  Menu {
    id: systemMenu
    title: 'System Menu'
    type: Menu.MenuType.Submenu
    MenuItem {
      title: 'About System...'
    }
    MenuItem {
      title: ''
      separator: true
    }
    MenuItem {
      title: 'System Preferences...'
    }
    MenuItem {
      title: ''
      separator: true
    }
    MenuItem {
      title: 'Test'
      onTriggered: {
        print(Shell.networkManager.currentConnectionId);
        for (let i = 0; i < Shell.networkManager.knownAps.length; ++i) {
          print(Shell.networkManager.knownAps[i].ssid);
        }
      }
    }
    MenuItem {
      title: 'Quit'
      onTriggered: {
        Shell.quit();
      }
    }
  }
}
