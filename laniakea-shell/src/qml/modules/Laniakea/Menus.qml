pragma Singleton
import QtQuick 2.12

import Blusher 0.1

Item {
  property var systemMenu: Menu {
    title: 'System'
    type: Menu.MenuType.Submenu
    items: [
      {
        path: '/preferences',
        title: 'System Preferences',
        action: function() {
          Shell.systemPreferences.show();
        }
      },
      {
        path: '/separator1',
        title: '---',
        separator: true
      },
      {
        path: '/lock',
        title: 'Lock'
      },
      {
        path: '/shut-down',
        title: 'Shut Down'
      },
      {
        path: '/debug-quit',
        title: '(DEBUG) Quit Laniakea Shell',
        action: function() {
          Shell.quit();
        }
      }
    ]
  } // Menu
}
