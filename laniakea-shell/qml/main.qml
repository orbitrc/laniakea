import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Window 2.12 as QtQuickWindow

import Blusher 0.1
import Blusher.DesktopEnvironment 0.1

Window {
  id: root

  visible: true
  netWmStrutPartial: [
    0,
    0,
    DesktopEnvironment.primaryScreen.y + 30,
    0, 0, 0, 0, 0, 0, 1000, 0, 0
  ]
  netWmWindowType: Window.NetWmWindowType.Dock
  onAllDesktops: true

  x: DesktopEnvironment.primaryScreen.x
  y: DesktopEnvironment.primaryScreen.y
  width: DesktopEnvironment.primaryScreen.width
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
      id: logoImage
      source: 'qrc:/assets/orbit-logo-light@256x256.png'
      anchors.centerIn: parent
      width: 30
      height: 30
    }
    ColorOverlay {
      anchors.fill: logoImage
      source: logoImage
      color: '#000000'
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
  // Run command debug button
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

  //=======================
  // Menu bar extensions
  //=======================
  Row {
    id: menuBarExtensions

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right

    layoutDirection: Qt.RightToLeft

    // Clock
    MenuBarExtension {
      id: clockExtension

      text: '00:00:00'
    }

    // Battery
    MenuBarExtension {
      text: Shell.batteryLevel + '%'
    }

    // Desktops
    MenuBarExtension {
      text: Shell.desktopName(Shell.currentDesktop)
      menu: desktopsMenu
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

  Menu {
    id: desktopsMenu
    title: 'Desktops Menu'
    type: Menu.MenuType.Submenu

    Component.onCompleted: {
      for (let i = 0; i < Shell.numberOfDesktops; ++i) {
        let item = Qt.createQmlObject('import Blusher 0.1\n'
          + 'MenuItem {\n'
          + '    title: Shell.desktopName(' + (i + 1) + ')\n'
          + '}', desktopsMenu, '');
        desktopsMenu.addItem(item);
      }
    }
  }

  RunCommandPopup {
    id: runCommand
  }

  //==================
  // Desktop windows
  //==================
  Repeater {
    id: desktops

    model: 1
    View {
      Window {
        visible: true
        netWmWindowType: Window.NetWmWindowType.Desktop

        width: 1920
        height: 1080

        color: 'lightgray'

        Image {
          anchors.fill: parent

          source: 'file:/' + Shell.wallpaper
          onStatusChanged: {
            if (status === Image.Error) {
              print('Failed to load wallpaper image.');
              this.visible = false;
            } else if (status === Image.Ready) {
              this.visible = true;
            }
          }
        }
      }
    }
  }

  //=============
  // Timer
  //=============
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
      clockExtension.text = text;
    }
  }
}
