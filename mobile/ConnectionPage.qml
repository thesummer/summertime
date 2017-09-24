import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Mobile 1.0

Page {
    ColumnLayout {
        id: col
        anchors.horizontalCenter: parent.horizontalCenter

        WarningNetworkBox {
            id: warningNetwork
        }

        ConnectionButton {
            id: connectButton
            text: qsTr("Connect")

            onClicked: {
                remoteConnection.connectSocket(inputIp.text) }
        }

        MouseArea {
            id: buttonHideIp
            Layout.topMargin: 1
            Layout.preferredWidth: textIp.implicitWidth
            Layout.preferredHeight: textIp.implicitHeight
            state: "collapsed"

            Text {
                id: textIp
                text: qsTr("[+] Set custom ip")
                color: "green"
            }

            states: [
                State {
                    name: "collapsed"
                    PropertyChanges { target: textIp; text: qsTr("[+] Set custom ip") }
                    PropertyChanges { target: inputIp; visible: false }
                },
                State {
                    name: "open"
                    PropertyChanges { target: textIp; text: qsTr("[-] Set custom ip") }
                    PropertyChanges { target: inputIp; visible: true }
                }
            ]

            onClicked: { state = state == "collapsed" ? "open" : "collapsed" }
        }
        Rectangle {
            id: inputIp
            property string text: inputIpText.text == "..." ? "127.0.0.1" : inputIpText.text
            border.width: 1
            Layout.leftMargin: 25
            Layout.preferredHeight: inputIpText.implicitHeight * 1.2
            Layout.preferredWidth: inputIpText.implicitWidth * 1.2
            TextInput {
                id: inputIpText
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                inputMask: "000.000.000.000;_"
                font.pointSize: 14
                selectByMouse: true
            }
        }
        ConnectionButton {
            id: disconnectButton
            enabled: false
            text: qsTr("Disconnect")

            onClicked: { remoteConnection.disconnectSocket() }
        }

        Connections {
            target: remoteConnection
            onStatusChanged: {
            switch (newStatus) {
            case RemoteConnection.Connected: col.state = "connected"
                break;
            case RemoteConnection.Disconnected: col.state = "disconnected"
                break;
            case RemoteConnection.ErrInvalidAddress:
                break;
            }
            }
        }

        states: [
            State {
                name: "disconnected"
                PropertyChanges { target: connectButton; enabled: true; }
                PropertyChanges { target: disconnectButton; enabled: false; }
            },
            State {
                name: "connected"
                PropertyChanges { target: connectButton; enabled: false; }
                PropertyChanges { target: disconnectButton; enabled: true; }
            }

        ]
    }
}