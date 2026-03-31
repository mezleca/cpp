import osu_stuff 3.0

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ApplicationWindow {
    id: root
    visible: true
    width: 1200
    height: 800
    title: "osu-stuff"

    // Tabs background
    Rectangle {
        width: root.width
        height: row.implicitHeight
        color: Theme.bg_color

        // Tabs layout
        RowLayout {
            id: row
            width: root.width
            spacing: 4

            // spacer
            Item { implicitWidth: 16 }

            // Draw Title
            Text {
                text: "osu-stuff"
                color: Theme.accent_primary
                font.bold: true
                font.pixelSize: 14
            }

            // spacer
            Item { implicitWidth: 16 }

            // Draw each button next to each other
            Repeater {
                model: app.tabs
                TabButton {
                    required property int index
                    required property string modelData

                    text: modelData
                    selected: app.current_tab == index
                    padding: 6
                    onClicked: {
                        app.set_current_tab(index)
                    }
                }
            }

            // then fill the rest of the remaining width
            Item { Layout.fillWidth: true }

            // spacer
            Item { implicitWidth: 16 }
        }
    }

    // Render tab contents
}
