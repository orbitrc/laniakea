import QtQuick 2.12
import QtQuick.Window 2.12 as QtQuickWindow

import Blusher 0.1

QtQuickWindow.Window {
  id: root
  flags: Qt.FramelessWindowHint

  x: 100
  y: 50
  width: 500
  height: 50

  color: "grey"

  Item {
    focus: true
    TextField {
      id: textField

      text: ''
    }

    Keys.onPressed: {
      if (event.key === Qt.Key_Escape) {
        root.close();
      } else if (event.key === Qt.Key_Return) {
        Shell.runCommand(textField.text);
        root.close();
      }
    }
  }

  Component.onCompleted: {
  }

  onVisibleChanged: {
    if (!visible) {
      textField.text = '';
    }
  }
}
