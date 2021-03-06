#ifndef ENTROPYCHARTVIEW_H
#define ENTROPYCHARTVIEW_H

/*
 * Specialized chart class for entropy & extremums density diagram
 */
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChart>
#include <QGraphicsLayout>

#include "entropyflowwindow.h"

class EntropyChartView : public QtCharts::QChartView
{
    Q_OBJECT
public:
    explicit EntropyChartView(QWidget *parent = 0);
    explicit EntropyChartView(QVector<EntropyY> &points, QWidget *parent = 0);

    void clear();
    bool add_points(QVector<EntropyY> &points);

signals:

public slots:
};

typedef EntropyChartView ExtremumDensityChartView;

#endif // ENTROPYCHARTVIEW_H
