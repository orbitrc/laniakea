import QtQuick 2.12

import Blusher 0.1

import "src/modules"

Rectangle {
  id: root

  color: "grey"

  property var systemMenu: Menus.systemMenu

  property string clock: ''
  MouseArea {
    width: 32
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    Rectangle {
      anchors.fill: parent
      color: "red"
    }
    onClicked: {
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
  }

  Timer {
    interval: 1000
    running: true
    repeat: true
    onTriggered: {
      root.clock = new Date();
    }
  }

  Component.onCompleted: {
    root.clock = new Date();
    print(Menus.msg);
  }
}
