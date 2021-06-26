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
    id: systemMenuItem

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
      width: 36
      height: root.height
      anchors.centerIn: parent
      color: "transparent"
    }
    Image {
      id: logoImage
      source: 'qrc:/assets/orbit-logo-light@256x256.png'
      anchors.centerIn: parent
      anchors.verticalCenter: parent
      width: 24
      height: 24
    }
    ColorOverlay {
      anchors.fill: logoImage
      source: logoImage
      color: systemMenuItem.state !== 'active' ? '#000000' : '#ffffff'
    }

    MouseArea {
      anchors.fill: parent

      onClicked: {
        systemMenu.open(0, root.height);
        parent.state = 'active';
      }
    }

    Connections {
      target: systemMenu
      function onClosing() {
        systemMenuItem.state = '';
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

  // Debug text
  Text {
    id: debugText

    x: 370
    text: JSON.stringify(DesktopEnvironment.screens) + '\n' + root.x + ', ' + root.y + ' ' + root.width + 'x' + root.height
    font.pixelSize: 10
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
      onTriggered: {
        aboutSystemWindowLoader.sourceComponent = aboutSystemWindow;
      }
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
        let item = desktopsMenuItem.createObject(this, {
          title: Shell.desktopName(i + 1),
          index: i,
        });
        desktopsMenu.addItem(item);
      }
    }
  }

  Component {
    id: desktopsMenuItem

    MenuItem {
      property string title
      property int index

      title: this.title
      onTriggered: {
        console.log(this.index);
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

    model: DesktopEnvironment.screens.length
    View {
      Window {
        visible: true
        netWmWindowType: Window.NetWmWindowType.Desktop

        title: 'laniakea-shell (Desktop)'

        x: DesktopEnvironment.screens[index].x
        y: DesktopEnvironment.screens[index].y + 1  // To bypass cropped by struts bug.
        width: DesktopEnvironment.screens[index].width
        height: DesktopEnvironment.screens[index].height

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
        Component.onCompleted: {
          // Restore position that +1 on created.
          this.y = DesktopEnvironment.screens[index].y;
        }
      }
    }
  }

  //=============
  // Windows
  //=============
  Component {
    id: aboutSystemWindow

    AboutSystemWindow {
//      onClosing: {
//        aboutSystemWindowLoader.sourceComponent = undefined;
//      }
      onVisibleChanged: {
        if (this.visible === false) {
          aboutSystemWindowLoader.sourceComponent = undefined;
        }
      }
    }
  }

  //=================
  // Window loaders
  //=================
  Loader {
    id: aboutSystemWindowLoader
  }

  //==============
  // Animation
  //==============
  NumberAnimation on y {
    from: -root.height
    to: 0
    duration: 500
  }

  //=============
  // Timer
  //=============
  Timer {
    interval: 1000
    running: true
    repeat: true
    onTriggered: {
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
