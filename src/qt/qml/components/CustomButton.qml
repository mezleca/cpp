import osu_stuff 3.0

import QtQuick
import QtQuick.Controls

Button {
    // styling
    property color text_color: Theme.text_color
    property color accent_color: Theme.accent_primary
    property color bg_color: Theme.bg_secondary
    property color border_color: Theme.border_primary

    property int border_width: 2
    property int border_radius: 4

    // extra
    property bool loading: false
    property bool selected: false

    property int font_size: 13

    id: root
    text: "button"

    enabled: !loading

    background: Rectangle {
        color: root.pressed ? Qt.darker(root.bg_color, 1.3)
             : root.hovered ? Qt.lighter(root.bg_color, 1.3)
             : root.bg_color

        border.width: root.border_width
        border.color: root.selected ? root.accent_color
                    : root.hovered ? root.border_color
                    : root.bg_color
        radius: root.border_radius
    }

    contentItem: Text {
        text: root.loading ? "loading" : root.text
        color: root.text_color
        font.pixelSize: root.font_size
        font.bold: true
    }
}
