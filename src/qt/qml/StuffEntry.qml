pragma ComponentBehavior: Bound

import osu_stuff 3.0

import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property string active_tab: "index"
    
    property var tabs: ["index", "collections", "browse", "discover", "radio", "settings"]
    property var tabs_table: ({
        "index":       () => Qt.createComponent("tabs/IndexTab.qml"),
        "collections": () => Qt.createComponent("tabs/CollectionsTab.qml"),
        "browse":      () => Qt.createComponent("tabs/BrowseTab.qml"),
        "discover":    () => Qt.createComponent("tabs/DiscoverTab.qml"),
        "radio":       () => Qt.createComponent("tabs/RadioTab.qml"),
        "settings":    () => Qt.createComponent("tabs/SettingsTab.qml")
    })

    AppController { id: stuff }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // header
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: header_row.implicitHeight

            color: Theme.bg_secondary

            RowLayout {
                id: header_row
                width: parent.width
                spacing: 4

                Item { implicitWidth: 16 }

                // title
                Text {
                    text: "osu-stuff"
                    color: Theme.accent_primary
                    font.bold: true
                    font.pixelSize: 16
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.active_tab = "index"
                    }
                }

                Item { implicitWidth: 16 }

                // tab buttons
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
                        onClicked: root.active_tab = modelData
                    }
                }

                Item { Layout.fillWidth: true }
                Item { implicitWidth: 16 }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: Theme.border_secondary
            }
        }

        // tab content
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Repeater {
                model: root.tabs
                delegate: Loader {
                    required property int index
                    required property string modelData

                    anchors.fill: parent
                    sourceComponent: root.tabs_table[modelData]()
                    visible: root.active_tab === modelData
                    active: visible || item !== null
                }
            }
        }
    }
}