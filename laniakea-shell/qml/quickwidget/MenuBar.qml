import QtQuick 2.12

import Blusher 0.1

View {
  id: menuBar

  width: 1500
  height: 30

  property string clock: ''

  signal clockClicked()
  signal openMenu(var items)

  Rectangle {
    id: rect

    anchors.fill: parent

    color: "#181818"

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
        menuBar.clock = text;
      }
    }

    //================
    // System menu
    //================
    Item {
      width: 50
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      Text {
        text: 'System'
        color: "white"
      }
      Menu {
        id: systemMenu
        title: 'System Menu'
        type: Menu.MenuType.Submenu
        MenuItem {
          title: 'Test'
          onTriggered: {
            print(Shell.networkManager.currentConnectionId);
          }
        }
        MenuItem {
          title: 'Quit'
          onTriggered: {
            Shell.quit();
          }
        }
      }

      MouseArea {
        anchors.fill: parent

        onClicked: {
          systemMenu.open(0, menuBar.height);
        }
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

      layoutDirection: Qt.RightToLeft

      // Clock
      Item {
        width: 90
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        Text {
          text: menuBar.clock
          color: "white"
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            print('clock clicked');
            menuBar.clockClicked();
          }
        }
      }

      // Battery
      Item {
        width: batteryText.implicitWidth + 10
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        Text {
          id: batteryText
          text: Shell.batteryLevel + '%'
          color: "white"
        }
      }

      // Desktops
      Item {
        width: desktopText.implicitWidth + 10
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        Text {
          id: desktopText
          text: Shell.desktopName(Shell.currentDesktop)
          color: "white"
        }
      }
    }
  }
}

