import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Mobile 1.0

Page {
    ColumnLayout {
        id: col
        anchors.horizontalCenter: parent.horizontalCenter

        ConnectionButton {
            id: connectButton
            text: qsTr("Connect")

            onClicked: {
                remoteConnection.connectDevice()
            }
        }

        ConnectionButton {
            id: disconnectButton
            enabled: false
            text: qsTr("Disconnect")

            onClicked: { remoteConnection.disconnect() }
        }

        WarningNetworkBox {
            id: warningNetwork
        }

        Connections {
            target: remoteConnection
            onStatusChanged: {
            switch (newStatus) {
            case RemoteConnection.Connected:

            case RemoteConnection.ActiveMeasurement:
                col.state = "connected"
                warningNetwork.text = qsTr("Connected to Sommerzeit")
                warningNetwork.color = "green"
                break;
            case RemoteConnection.Disconnected:
                col.state = "disconnected"
                warningNetwork.text = qsTr("Not connected to 'Sommerzeit'")
                warningNetwork.color = "orange"
                break;
            case RemoteConnection.BluetoothDisabled:
                col.state = "disconnected"
                warningNetwork.text = qsTr("Bluetooth is disabled")
                warningNetwork.color = "red"
                break;
            case RemoteConnection.DeviceNotFound:
                col.state = "disconnected"
                warningNetwork.text = qsTr("Device not found")
                warningNetwork.color = "red"
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
