import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Button {
    id: connectButton
    Layout.topMargin: 5
    Layout.preferredWidth: 300
    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    height: 70
    
    text: qsTr("Connect")
    font.pointSize: height * 0.3
}
