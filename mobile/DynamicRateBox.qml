import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

MouseArea{
    id: mouseArea
    Layout.fillHeight: true
    Layout.fillWidth: true

    property color color: "white"
    property real value: 0.0
    property string unit: ""

    Rectangle
    {
        anchors.fill: parent
        color : parent.color
        Column {
            anchors.fill: parent
            Label {
                height: 0.75 * parent.height
                width: parent.width
                text: value.toFixed(2)
                font.pointSize: height * 0.5
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            Label {
                height: 0.25 * parent.height
                width: parent.width
                text: unit
                font.pointSize: height * 0.5
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
