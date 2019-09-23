import QtQuick 2.12

import Blusher 0.1
import Blusher.DesktopEnvironment 0.1

Item {
  id: root

  property string title: ''
  property bool bold: false
  property int fixedWidth: 0
  property bool focused: false

  signal triggered(int x)

  width: rect.width

  Rectangle {
    id: rect
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: root.fixedWidth > 0 ? root.fixedWidth : text.implicitWidth
    color: root.focused ? "blue" : "#00ffffff"

    Label {
      id: text

      anchors.fill: rect
      text: root.title
      anchors.verticalCenter: parent.verticalCenter
//      rightPadding: 7.0 * DesktopEnvironment.pixelsPerDp
//      leftPadding: 7.0 * DesktopEnvironment.pixelsPerDp
      fontSize: 14
      fontColor: "white"
//      backgroundColor: "green"
//      font.bold: root.bold
    }

    MouseArea {
      anchors.fill: parent
      onPressed: {
        var pos = mapToGlobal(this.x, this.y);
        root.triggered(pos.x);
      }

//      onClicked: {
//        root.triggered();
//      }
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
