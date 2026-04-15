import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    title: "osu-stuff"
    visible: true
    width: 1200
    height: 800
    minimumWidth: 800
    minimumHeight: 600

    StuffEntry {
        width: window.width
        height: window.height 
    }
}
