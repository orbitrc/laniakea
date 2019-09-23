import QtQuick 2.12

import Blusher 0.1
import Blusher.DesktopEnvironment 0.1

Window {
  id: root

  enum Category {
    All,
    Desktop,
    Display,
    Keyboard,
    Localization
  }

  property int category: SystemPreferences.Category.All


  type: Window.WindowType.AppWindow
  title: 'System Preferences'

  windowWidth: 600
  windowHeight: 400

  toolbar: Toolbar {
    ToolbarItem {
      label: 'Show all'
      SegmentedControl {
        trackingMode: SegmentedControl.TrackingMode.Momentary
        Segment {
          label: 'All'
        }

        onSelected: {
          if (index === 0) {    // All
            root.category = SystemPreferences.Category.All;
          }
        }
      }
    }
  }

  body: root.all

  onCategoryChanged: {
    print('category: ' + root.category);
    switch (root.category) {
    case SystemPreferences.Category.All:
      root.body = root.all;
      break;
    case SystemPreferences.Category.Desktop:
      root.body = root.desktop;
      break;
    case SystemPreferences.Category.Display:
      root.body = root.display;
      break;
    case SystemPreferences.Category.Keyboard:
      root.body = root.keyboard;
      break;
    case SystemPreferences.Category.Localization:
      root.body = root.localization;
      break;
    default:
      break;
    }
    print(root.body);
  }

  onVisibleChanged: {
    if (root.visible === false) {
      root.category = SystemPreferences.Category.All;
    }
  }


  //=========
  // All
  //=========
  property Item all: Item {
    Column {
      // Desktop Behavior / Look and Feel
      Row {
        Button {
          title: 'Desktop'
          onClicked: {
            root.category = SystemPreferences.Category.Desktop;
          }
        }
      }
      // Language / Region
      Row {
        Button {
          title: 'Language'
          onClicked: {
            root.category = SystemPreferences.Category.Localization;
          }
        }
      }

      // Input / Output
      Row {
        Button {
          title: 'Display'
          onClicked: {
            root.category = SystemPreferences.Category.Display;
          }
        }
        Button {
          title: 'Keyboard'
          onClicked: {
            root.category = SystemPreferences.Category.Keyboard;
          }
        }
      }
    } // Column
  }

  //=================
  // Desktop
  //=================
  property Item desktop: Item {
    Label {
      width: 200
      text: 'Number of virtual desktops: '
    }
    Button {
      title: '-'
      onClicked: {
        const num = Shell.preferences.desktop.numberOfDesktops;
        if (num > 1) {
          Shell.preferences.desktop.numberOfDesktops -= 1;
        }
      }
    }
    Label {
      y: 50
      width: 50
      text: Shell.preferences.desktop.numberOfDesktops
    }
    Button {
      x: 200
      title: '+'
      onClicked: {
        Shell.preferences.desktop.numberOfDesktops += 1;
      }
    }
  }

  //===============
  // Display
  //===============
  property Item display: Item {

  }

  //===============
  // Keyboard
  //===============
  property Item keyboard: Item {
    Column {
      Row {
        Label {
          text: 'Delay until repeat: '
        }
        Button {
          title: '-'
          onClicked: {
            const delay = Shell.preferences.keyboard.delayUntilRepeat;
            if (delay > 0) {
              Shell.preferences.keyboard.delayUntilRepeat -= 100;
            }
          }
        }
        Label {
          text: Shell.preferences.keyboard.delayUntilRepeat
        }
        Button {
          title: '+'
          onClicked: {
            const delay = Shell.preferences.keyboard.delayUntilRepeat;
            if (delay < 2000) {
              Shell.preferences.keyboard.delayUntilRepeat += 100;
            }
          }
        }
      }
      Row {
        Label {
          text: 'Repeat rate: '
        }
        Button {
          title: '-'
        }
        Label {
          text: Shell.preferences.keyboard.keyRepeat
        }
        Button {
          title: '+'
        }
      }
    }
  }

  //===================
  // Localization
  //===================
  property Item localization: Item {
    Text {
      text: 'l10n'
    }
  }
}
