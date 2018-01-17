import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Mobile 1.0

ColumnLayout {
    id: col
    height: parent.height
    state: "disconnected"

    property real displayHeight : height * 0.85
    function moveToTop(newTop) {
        if (newTop.parent !== mainRate) {
            var tmp = mainRate.children[0]
            tmp.parent = additionalRates
            newTop.parent = mainRate
            newTop.width = newTop.parent.width
            tmp.width = tmp.parent.width / 2
        }
    }

    Connections {
        target: remoteConnection
        onStatusChanged: {
        switch (newStatus) {
        case RemoteConnection.Connected:
            col.state = "idle"
            break;
        case RemoteConnection.Disconnected:
            col.state = "disconnected"
            break;
        case RemoteConnection.ActiveMeasurement:
            col.state = "activeMeasurement"
            break;
        case RemoteConnection.ErrInvalidAddress:
            break;
        }
        }
    }

    states: [
        State {
            name: "disconnected"
            PropertyChanges {
                target: measurementButton; enabled: false }
        },
        State {
            name: "idle"
            PropertyChanges {
                target: measurementButton; enabled: true; text: qsTr("Start measurement")
            }
        },
        State {
            name: "activeMeasurement"
            PropertyChanges { target: measurementButton; text: qsTr("Stop measurement") }
        }
    ]

    Row {
        id: mainRate
        Layout.fillWidth: true
        Layout.preferredHeight: 2* parent.displayHeight / 3

        populate: Transition {
            NumberAnimation { properties: "x,y"; from: 200; duration: 100; easing.type: Easing.OutBounce }
        }
        add: Transition {
            NumberAnimation { properties: "x"; easing.type: Easing.OutQuad }
        }
        move: Transition {
            NumberAnimation { properties: "x"; easing.type: Easing.OutBounce }
        }

        DynamicRateBox {
            id: boxPerMinute
            unit: qsTr("per minute")
            width: parent.width
            height: parent.height
            Layout.columnSpan: 2
            color: "#80c342"
        }
    }

    Row {
        id: additionalRates
        Layout.preferredHeight: parent.displayHeight / 3
        Layout.fillWidth: true

        populate: Transition {
            NumberAnimation { properties: "x,y"; from: 200; duration: 100; easing.type: Easing.OutBounce }
        }
        add: Transition {
            NumberAnimation { properties: "x"; easing.type: Easing.OutQuad }
        }
        move: Transition {
            NumberAnimation { properties: "x"; easing.type: Easing.OutBounce }
        }

        DynamicRateBox {
            id: boxPerHour
            unit: qsTr("per hour")
            height: parent.height
            width: parent.width / 2
            color: "#006325"
        }

        DynamicRateBox {
            id: boxPerDay
            unit: qsTr("per day")
            height: parent.height
            width: parent.width / 2
            color: "#14aaff";
        }
    }

    Button {
        id: measurementButton
        Layout.preferredWidth: parent.width * 0.6
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        Layout.preferredHeight: 70
        text: qsTr("Start measurement")
        font.pointSize: height * 0.3
        onClicked: {
            if (parent.state == "idle")
            {
                remoteConnection.startMeasurement();
            }
            else if (parent.state == "activeMeasurement")
            {
                remoteConnection.stopMeasurement();
            }
        }
    }

    Connections {
        target: remoteConnection
        onUpdateLiveData: {
            boxPerMinute.value = newRate * 60.0
            boxPerHour.value   = boxPerMinute.value * 60.0
            boxPerDay.value    = boxPerHour.value * 24.0
        }
    }

    Connections {
        target: boxPerMinute
        onClicked: { col.moveToTop(target) }
    }

    Connections {
        target: boxPerHour
        onClicked: { col.moveToTop(target) }
    }

    Connections {
        target: boxPerDay
        onClicked: { col.moveToTop(target) }
    }
}
