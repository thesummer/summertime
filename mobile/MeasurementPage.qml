import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

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
