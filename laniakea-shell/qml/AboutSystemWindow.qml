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
}
