pragma ComponentBehavior: Bound

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

    property string active_tab: "index"
    property var tabs: ["index", "collections", "browse", "discover", "radio", "settings"]

    Component { id: index_comp; IndexTab {} }
    Component { id: collections_comp; CollectionsTab {} }
    Component { id: browse_comp; BrowseTab {} }
    Component { id: discover_comp; DiscoverTab {} }
    Component { id: radio_comp; RadioTab {} }
    Component { id: settings_comp; SettingsTab {} }

    property var tabs_table: {
        "index": index_comp,
        "collections": collections_comp,
        "browse": browse_comp,
        "discover": discover_comp,
        "radio": radio_comp,
        "settings": settings_comp
    }

    AppController {
        id: stuff
    }

    // Background
    Rectangle {
        width: root.width
        height: root.height

        color: Theme.bg_primary
    }

    // App Layout
    ColumnLayout {
        id: app_layout

        Layout.fillWidth: true
        Layout.fillHeight: true

        // Tabs header
        Rectangle {
            implicitWidth: root.width
            implicitHeight: row.implicitHeight
            color: Theme.bg_secondary

            RowLayout {
                id: row
                width: root.width
                spacing: 4

                Item { implicitWidth: 16 } // spacer

                // Draw Title
                Text {
                    text: "osu-stuff"
                    color: Theme.accent_primary
                    font.bold: true
                    font.pixelSize: 15

                    // TODO: replace with TitleComponent
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.active_tab = "index"
                        }
                    }
                }

                Item { implicitWidth: 16 } // spacer

                // Draw tab buttons
                Repeater {
                    model: root.tabs
                    StuffButton {
                        required property int index
                        required property string modelData

                        visible: modelData !== "index"
                        text: modelData
                        selected: root.active_tab === modelData
                        padding: 6
                        hover_color: false
                        onClicked: {
                            root.active_tab = modelData
                        }
                    }
                }

                // Fill th rest of the remaining width
                Item { Layout.fillWidth: true }

                // Spacer
                Item { implicitWidth: 16 }
            }

            // Bottom border
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: Theme.border_color
            }
        }

        // Tab content
        Rectangle {
            id: tab_container

            Layout.fillWidth: true
            Layout.fillHeight: true

            Repeater {
                model: root.tabs

                delegate: Loader {
                    required property int index
                    required property string modelData

                    sourceComponent: root.tabs_table[modelData]
                    visible: root.active_tab === modelData
                    active: visible || item !== null
                }
            }
        }
    }
}
