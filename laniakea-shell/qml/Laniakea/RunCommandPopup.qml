import QtQuick 2.12

import Blusher 0.1

Window {
  id: root

  title: 'Run Command'

  property string command: _text.text

  flags: Qt.Tool | Qt.FramelessWindowHint

  width: 320
  height: 50

//  TextField {
//  }
  TextInput {
    id: _text
    anchors.fill: parent
  }

  onKeyPressed: {
    if (event.key == Qt.Key_Escape) {
      root.close();
    }
    if (event.key == Qt.Key_Return) {
      Shell.runCommand(root.command);
    }
  }

  // onActiveChanged directly causes an error message.
  Connections {
    target: root
    onActiveChanged: {
      if (root.active == false) {
        root.close();
      }
    }
  }
}
