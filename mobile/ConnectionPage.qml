import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

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

            onClicked: { parent.state = "connected" }
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

            onClicked: { parent.state = "disconnected" }
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
