import QtQuick 2.12

import Blusher.DesktopEnvironment 0.1

Item {
  id: root

  property string title: ''
  property bool bold: false

  signal triggered()

  width: rect.width

  Rectangle {
    id: rect
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: text.implicitWidth
    color: "yellow"

    Text {
      id: text
      text: root.title
      anchors.verticalCenter: parent.verticalCenter
      rightPadding: 7.0 * DesktopEnvironment.pixelsPerDp
      leftPadding: 7.0 * DesktopEnvironment.pixelsPerDp
      font.pixelSize: 14 * DesktopEnvironment.pixelsPerDp
      font.bold: root.bold
    }

    MouseArea {
      anchors.fill: parent
      onClicked: {
        root.triggered();
      }
    }
//    PopUpMenuDelegate {
//      id: systemMenuDelegate
//      menu: root.systemMenu // still be used?

//      path: '/'
//      items: root.systemMenu.items
//      onItemTriggered: {
//        if (systemMenuDelegate.menu.items[idx].action) {
//          systemMenuDelegate.menu.items[idx].action();
//        }
//      }
//    }
  }
}
