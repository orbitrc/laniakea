import QtQuick 2.12

import Blusher 0.1

import LaniakeaShell 0.1

Rectangle {
  id: root

  enum ItemType {
    MenuItem,
    MenuBarExtension
  }

  enum VisualType {
    TitleOnly,
    ImageOnly,
    TitleAndImage
  }

  property int itemType: MenuBarItemDelegate.ItemType.MenuItem
  property int visualType: MenuBarItemDelegate.VisualType.TitleOnly

  property string title: ''
  property string image: ''

  property var menuItems: []

  property bool focused: false

  height: 30
  width: root.visualType !== MenuBarItemDelegate.VisualType.ImageOnly ? _title.implicitWidth : 30
  color: root.focused ? "blue" : "#00ffffff"

  Item {
    Label {
      id: _title

      text: root.title
      fontSize: 14
      fontColor: "#ffffff"
    }
    Item {
      id: _imageFrame

      width: 30
      height: 30

      Image {
        id: _image

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        visible: root.visualType !== MenuBarItemDelegate.VisualType.TitleOnly
        source: root.image
        width: 24
        height: 24
      }
    }
  }

  PopUpMenuDelegate {
    id: _popUp

    path: '/'
    items: root.menuItems

    menuBarRect.x: 0
    menuBarRect.y: 0
    menuBarRect.width: 1024
    menuBarRect.height: 30
  }

  MouseArea {
    anchors.fill: parent

    onPressed: {
      let pos = mapToGlobal(root.x, root.y);
      _popUp.show(pos.x, pos.y);
    }
  }
}
