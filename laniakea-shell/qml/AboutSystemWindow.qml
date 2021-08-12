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

  View {
    id: distIconView

    x: 10
    y: 10
    width: 64
    height: 64
    Image {
      id: distIcon

      anchors.fill: parent
      source: Shell.systemInformation.distIcon
    }
  }

  Label {
    id: distribution

    anchors.left: distIconView.right
    anchors.leftMargin: 12
    anchors.verticalCenter: distIconView.verticalCenter

    text: Shell.systemInformation.distName
    fontSize: 32
  }

  View {
    anchors.top: distribution.bottom

    Flow {
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
}
