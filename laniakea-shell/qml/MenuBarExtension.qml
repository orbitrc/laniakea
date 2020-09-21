import QtQuick 2.12

import Blusher 0.1

View {
  id: root

  property string text
  property string imageSource: ''

  width: extensionText.implicitWidth + 10
  height: 30

  Text {
    id: extensionText
    text: root.text
  }
}
