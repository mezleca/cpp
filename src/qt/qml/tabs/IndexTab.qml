import osu_stuff 3.0

import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: Theme.bg_primary

    property int count: 0

    Button {
        id: count_btn
        text: "count: " + root.count
        onClicked: {
            root.count++;
        }
    }
}
