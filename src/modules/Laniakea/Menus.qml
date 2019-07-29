pragma Singleton
import QtQuick 2.12

import Blusher 0.1

Item {
  property string msg: 'Menus.msg: "hello"'
  property Menu systemMenu: Menu {
    title: 'System'
    type: Menu.MenuType.Submenu
    MenuItem {
      title: 'Preferences'
    }
  }
}
