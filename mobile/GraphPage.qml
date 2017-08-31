import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtCharts 2.2

Page {

    ChartView {
        id: chartView
        title: "Line"
        anchors.fill: parent
        antialiasing: true
        legend.visible: false

        ValueAxis {
            id: xAxis
            min: 0
        }

        ValueAxis {
            id: yAxis
            min: 0
        }

        LineSeries {
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
            id: scatter
            name: "Scatter1"
            color: "blue"
            axisX: xAxis
            axisY: yAxis
        }

        MouseArea {
            anchors.fill: parent
            onClicked: { scatter.visible = !scatter.visible }
        }
    }

    Timer {
        id: refreshTimer
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            dataSource.update(chartView.series(0));
            dataSource.update(chartView.series(1));
            dataSource.nextDataPoint();
            chartView.zoomReset()
        }
    }
}
