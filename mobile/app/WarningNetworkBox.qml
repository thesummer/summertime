import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Rectangle {
    Layout.topMargin: 50
    Layout.preferredWidth: warningText.implicitWidth
    Layout.preferredHeight: textIp.implicitHeight
    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
    Text {
        id: warningText
        text: qsTr("Not connected to network 'Sommerzeit'")
        color: "orange"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
}
