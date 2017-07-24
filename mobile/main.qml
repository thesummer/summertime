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
        spacing: 20
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        ConnectionPage {
        }

        MeasurementPage {
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("Connection")
        }
        TabButton {
            text: qsTr("Measurement")
        }
    }
}
