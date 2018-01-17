import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Item {
    id: item1
    property alias textField1: textField1
    property alias button1: button1

    RowLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        anchors.top: parent.top

        TextField {
            id: textField1
            placeholderText: qsTr("Text Field")
        }

        Button {
            id: button1
            text: qsTr("Press Me")
        }
    }

    GridLayout {
        id: gridLayout
        x: 100
        y: 107
        width: 495
        height: 320
    }

    Rectangle {
        id: rectangle
        x: 120
        width: 200
        height: 200
        color: "#ffffff"
        scale: 1.1
        anchors.top: gridLayout.bottom
        anchors.topMargin: -279
    }
}
