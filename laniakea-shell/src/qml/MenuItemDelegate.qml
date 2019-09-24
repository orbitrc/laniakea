import QtQuick 2.12
import QtQuick.Window 2.12 as QtQuickWindow

import Blusher 0.1
import Blusher.DesktopEnvironment 0.1

Item {
  id: root

  // C++ Properties
  property var self: null
  property var menuDelegate: null

  property int index: -1
  property var menuItem: null
  property string menuItemTitle: ' '
  property bool menuItemSeparator: false

  property bool focused: false

  width: _text.implicitWidth
  height: 30 * DesktopEnvironment.pixelsPerDp

  Rectangle {
    id: styler

    anchors.fill: parent
    border.width: 0
    color: root.focused ? "#bbb4b1" : "#d6d2d0"
    MouseArea {
      anchors.fill: parent
      hoverEnabled: true
      onEntered: {
        if (root.menuItemSeparator) {
          return;
        }
        if (root.self !== null) {
          root.self.itemHovered();
        }
      }
      onPositionChanged: {
        if (root.menuItemSeparator) {
          return;
        }
        if (!root.focused && root.self !== null) {
          root.self.itemHovered();
        }
      }

      onClicked: {
        if (root.menuItemSeparator) {
          return;
        }
        if (root.self !== null) {
          root.self.itemTriggered();
        }
      }
    }
  }

  Item {
    id: _checked

    width: 24
    height: 24

    anchors.verticalCenter: parent.verticalCenter
    anchors.leftMargin: 3 * DesktopEnvironment.pixelsPerDp
  }

  Text {
    id: _text
//    text: root.menuItemTitle
    text: root.menuItem ? root.menuItem.title : ' '
    visible: !root.menuItemSeparator
    font.pixelSize: 14 * DesktopEnvironment.pixelsPerDp
    anchors.verticalCenter: parent.verticalCenter
    leftPadding: _checked.width + (3 * DesktopEnvironment.pixelsPerDp)
  }

  Item {
    id: _separator
    visible: false
    height: 2 * DesktopEnvironment.pixelsPerDp
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter
    Rectangle {
      height: 1 * DesktopEnvironment.pixelsPerDp
      anchors.left: parent.left; anchors.right: parent.right;
      anchors.top: parent.top
      border.width: 0
      gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: "#00000000"; }
        GradientStop { position: 0.5; color: "grey"; }
        GradientStop { position: 1.0; color: "#00000000"; }
      }
    }
    Rectangle {
      height: 1 * DesktopEnvironment.pixelsPerDp
      anchors.left: parent.left; anchors.right: parent.right;
      anchors.bottom: parent.bottom
      border.width: 0
      gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: "#00000000"; }
        GradientStop { position: 0.5; color: "#ffffff"; }
        GradientStop { position: 1.0; color: "#00000000"; }
      }
    }
  }

  onMenuItemSeparatorChanged: {
    if (root.menuItemSeparator) {
      this.height = 10 * DesktopEnvironment.pixelsPerDp;
      _separator.visible = true
    }
  }

  onMenuItemChanged: {
    if (root.menuItem !== null && root.menuItem.separator === true) {
      root.menuItemSeparator = true;
    }
  }

  Component.onCompleted: {
  }
}
