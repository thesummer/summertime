#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QtCore/QObject>
#include <QtCharts/QAbstractSeries>

class QQuickView;

QT_CHARTS_USE_NAMESPACE

class DataSource : public QObject
{
    Q_OBJECT
public:
    explicit DataSource(QObject *parent = 0);

Q_SIGNALS:

public slots:

    void
    update(QAbstractSeries *series);

    void
    nextDataPoint();

private:
    QPointF
    newDataPoint(qreal x);

    qreal mX;
};

#endif // DATASOURCE_H
