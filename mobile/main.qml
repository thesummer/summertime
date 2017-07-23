import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: page
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        ColumnLayout {
            id: col
            height: parent.height
            Row {
                id: row1
                Layout.fillWidth: true
                Layout.preferredHeight: 2* parent.height / 3

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
                id: row2
                Layout.preferredHeight: parent.height / 3
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

            function moveToTop(newTop) {
                if (newTop.parent !== row1) {
                    var tmp = row1.children[0]
                    tmp.parent = row2
                    newTop.parent = row1
                    newTop.width = newTop.parent.width
                    tmp.width = tmp.parent.width / 2
                }
            }

            Connections {
                target: boxPerMinute
                onClicked: {
                    col.moveToTop(target)
                }
            }

            Connections {
                target: boxPerHour
                onClicked: {
                    col.moveToTop(target)
                }
            }

            Connections {
                target: boxPerDay
                onClicked: {
                    col.moveToTop(target)
                }
            }
        }

        Page {
            Label {
                text: qsTr("Second page")
                anchors.centerIn: parent
            }
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("Measurement")
        }
        TabButton {
            text: qsTr("Second")
        }
    }
}
