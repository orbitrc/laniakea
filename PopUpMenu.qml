import QtQuick 2.12

import Blusher 0.1

Rectangle {
  id: root

  property var menu: null

  width: 80
  height: 80
  color: "cyan"
  MouseArea {
    width: 70
    height: 70
    Rectangle {
      anchors.fill: parent
      color: "green"
    }
    onClicked: {
      print(root.menu);
      print(root.menu.items.length);
    }
  }
}
