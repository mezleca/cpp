import osu_stuff 3.0

import QtQuick
import QtQuick.Controls

Button {
    // styling
    property color text_color: Theme.text_color
    property color accent_color: Theme.accent_primary
    property color bg_color: Theme.bg_secondary
    property color border_color: Theme.border_primary

    property bool hover_color: true;

    property int border_height: 3

    // extra
    property bool selected: false
    property int font_size: 14

    id: root
    text: "button"

    enabled: true
    spacing: 32

    // Background
    background: Rectangle {
        color: root.hover_color ? root.pressed ? Qt.darker(root.bg_color, 1.3)
             : root.hovered ? Qt.lighter(root.bg_color, 1.3)
             : root.bg_color : root.bg_color
    }

    // Bottom border
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: root.border_height
        color: root.selected ? root.accent_color
             : root.hovered  ? Qt.darker(root.accent_color, 1.5)
             : "transparent"
    }

    contentItem: Text {
        text: root.text
        color: root.text_color
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: root.font_size
        font.bold: true
    }
}
