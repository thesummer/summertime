import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Rectangle {
    Layout.topMargin: 50
    Layout.preferredWidth: warningText.implicitWidth
    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
    property string text: qsTr("Not connected to 'Sommerzeit'")
    property color color: "orange"
    Text {
        id: warningText
        text: parent.text
        color: parent.color
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
}
