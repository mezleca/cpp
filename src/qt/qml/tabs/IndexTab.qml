import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property int count: 0

    Button {
        id: count_btn
        text: "count: " + root.count
        onClicked: {
            root.count++;
        }
    }
}
