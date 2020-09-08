import QtQuick 2.12
import QtQuick.Window 2.12 as QtQuickWindow

import Blusher 0.1

import "../Laniakea"

View {
  id: menuBar

  width: QtQuickWindow.Screen.width
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
    // Debug button
    MouseArea {
      width: 24
      x: 340
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      hoverEnabled: true
      Rectangle {
        id: _debugButton
        anchors.fill: parent
        color: "green"
      }
      onClicked: {
        runCommand.visible = true;
        runCommand.requestActivate();
      }
      onEntered: {
        _debugButton.color = "blue";
      }
      onExited: {
        _debugButton.color = "green";
      }
    }
    MouseArea {
      width: 24
      x: 360
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      hoverEnabled: true
      Rectangle {
        id: _debugButton2
        anchors.fill: parent
        color: "cyan"
      }
      onClicked: {
        print(QtQuickWindow.Screen.width);
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

      Menu {
        id: systemMenu
        title: 'System Menu'
        type: Menu.MenuType.Submenu
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

      MouseArea {
        anchors.fill: parent

        onClicked: {
          systemMenu.open(0, menuBar.height);
          parent.state = 'active';
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

    RunCommandPopup {
      id: runCommand
    }
  }
}

