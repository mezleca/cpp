import osu_stuff 3.0

import QtQuick
import QtQuick.Layouts

Rectangle {
    color: Theme.bg_primary

    GridLayout {
        anchors.left: parent.left
        anchors.top: parent.top

        rows: 2
        columns: 1

        // collections
        Rectangle {
            border.width: 1
            border.color: Theme.border_secondary

            ListModel {
                id: collections_modal
            }

            ListView {
                anchors.fill: parent
                model: collections_modal
                spacing: 5

                delegate: Rectangle {
                    anchors.margins: 8

                    width: parent.width
                    height: 40

                    Text {
                        id: collection_name
                        text: "name"
                    }

                    Text {
                        id: collection_count
                        text: "count"
                    }
                }
            }
        }

        // beatmap list
        Rectangle {

        }
    }
}
