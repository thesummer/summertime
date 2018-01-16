import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtCharts 2.2
import Mobile 1.0

Page {

    ChartView {
        id: chartView
        title: "Line"
        anchors.fill: parent
        antialiasing: true
        legend.visible: false

        property real currentMinute: 0.0

        Connections {
            target: remoteConnection
            onStatusChanged: {
            switch (newStatus) {
            case RemoteConnection.ActiveMeasurement:
                lineGraph.clear()
                scatterGraph.clear()
                chartView.currentMinute = 0.0
                break;
            default:
                break;
            }
            }
        }

        ValueAxis {
            id: xAxis
            min: 0
        }

        ValueAxis {
            id: yAxis
            min: 0
        }

        LineSeries {
            id: lineGraph
            name: "LineSeries"
            color: "blue"
            axisX: xAxis
            axisY: yAxis
            onPointAdded: {
                var point = at(index)
                xAxis.min = point.x < xAxis.min ? point.x : xAxis.min
                xAxis.max = point.x > xAxis.max ? point.x : xAxis.max
                yAxis.min = point.y < yAxis.min ? point.y : yAxis.min
                yAxis.max = point.y > yAxis.max ? point.y : yAxis.max
            }
        }

        ScatterSeries {
            id: scatterGraph
            name: "Scatter1"
            color: "blue"
            axisX: xAxis
            axisY: yAxis
        }

        MouseArea {
            anchors.fill: parent
            onClicked: { scatterGraph.visible = !scatterGraph.visible }
        }

        Connections {
            target: remoteConnection
            onAppendNewMinuteCount: {
                lineGraph.append(chartView.currentMinute, newCount)
                scatterGraph.append(chartView.currentMinute, newCount)
                chartView.zoomReset()
                chartView.currentMinute += 1.0

            }
        }
    }
}
