import QtQuick 2.12

import Blusher 0.1

Window {
  id: root

  visible: true

  netWmWindowType: Window.NetWmWindowType.Utility

  maximumWidth: 300
  maximumHeight: 300
  minimumWidth: this.maximumWidth
  minimumHeight: this.maximumHeight
  width: this.maximumWidth
  height: this.maximumHeight

  title: 'About System'

  Image {
    id: distIcon

    x: 10
    y: 10
    width: 64
    height: 64
    source: Shell.systemInformation.distIcon
  }

  Label {
    id: distribution

    x: 80
    y: 10

    text: Shell.systemInformation.distName
    fontSize: 32
  }

  Flow {
    anchors.top: distribution.bottom

    flow: Flow.TopToBottom

    Item {
      Label {
        id: labelKernel
        text: 'Kernel:'
      }
      Label {
        anchors.left: labelKernel.right
        text: Shell.systemInformation.kernel
        selectable: true
      }
    }
  }
}
