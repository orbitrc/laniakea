import QtQuick 2.0

import Blusher.DesktopEnvironment 0.1

Item {
  property var menuBar

  width: 200
  height: 30
  Row {
    anchors.fill: parent
    spacing: 0
    Rectangle {
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      width: 30 * DesktopEnvironment.pixelsPerDp
      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          parent.color = "#550000ff"
        }
      }
    }
    Repeater {
      id: menuItemRepeater
      model: 0
      delegate: Rectangle {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: text.implicitWidth
        color: "brown"
        Text {
          id: text
          visible: false
          text: Shell.menuBarMenu.items[index].title
        }
        MouseArea {
          anchors.fill: parent
          hoverEnabled: true
          onEntered: {
            parent.color = "red";
          }
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
    } // Repeater
  } // Row
}
