import QtQuick 2.12

import Blusher 0.1

View {
  id: root

  property string text
  property string imageSource: ''
  property Menu menu: null

  width: extensionText.implicitWidth + 10
  height: 30

  states: [
    State {
      name: 'active'
      PropertyChanges {
        target: activeColor
        color: 'blue'
      }
      PropertyChanges {
        target: extensionText
        color: 'white'
      }
    }
  ]

  Rectangle {
    id: activeColor

    anchors.fill: parent

    visible: root.menu != null
    color: 'transparent'
  }

  Text {
    id: extensionText

    anchors.centerIn: parent
    text: root.text
    font.pixelSize: 14
  }

  MouseArea {
    anchors.fill: parent

    onPressed: {
      if (menu != null) {
        let pos = mapToItem(root.parent.parent, root.x, 30);
        print(pos);
        menu.open(pos.x, pos.y);
        root.state = 'active';
      }
    }
  }

  Connections {
    target: root.menu
    function onClosing() {
      root.state = '';
    }
  }
}
