import QtQml 2.12
import QtQuick 2.12 as QtQuick

import Blusher 0.1

QtQuick.Item {
  id: root

  property string title: ''

  Label {
    text: root.title
    fontColor: "white"
  }
}
