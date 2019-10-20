import QtQml 2.12
import QtQuick 2.12 as QtQuick

import Blusher 0.1

QtQuick.Item {
  id: root

  property string title: ''
  property string image: ''

  Label {
    text: root.title
    fontColor: "white"
    visible: root.image === ''
  }
  QtQuick.Image {
    source: root.image
    visible: root.image !== ''
    width: 24
    height: 24
  }
}
