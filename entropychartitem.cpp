#include "entropychartitem.h"

EntropyChartItem::EntropyChartItem(QVector<EntropyY> &pts,
                                   QTreeWidgetItem *parent) : QTreeWidgetItem(parent),
    chartView(Q_NULLPTR)
{
    if(QTreeWidget *tree = treeWidget()) {
        chartView = new TreeChart(this, pts);
        tree->setItemWidget(this, 2, chartView);
        QObject::connect(tree,
                         &QTreeWidget::itemSelectionChanged,
                         chartView,
                         &TreeChart::selectedItem);
    }
}

EntropyChartItem::TreeChart::TreeChart(QTreeWidgetItem *ownerItem,
                                       QVector<EntropyY> &pts,
                                       QWidget *parent) : EntropyChartView(parent),
    owner_item(ownerItem)
{
    if(!add_points(pts))
        throw;
    //TODO: resize/hide some elements
    setFixedSize(200, 60);
    chart()->setMaximumHeight(60);
    chart()->setMaximumWidth(200);
    chart()->setMinimumHeight(60);
    chart()->setMinimumWidth(200);
    //chart()->setTheme(QtCharts::QChart::ChartTheme::ChartThemeLight);
    chart()->legend()->hide();
    chart()->axisX()->hide();
    chart()->axisY()->hide();
    chart()->setTitle(QString());

    //chart()->legend()->setVisible(false);

    setMouseTracking(true);

    chart()->setTheme(cDefault);
}

void EntropyChartItem::TreeChart::selectedItem() {
    chart()->setTheme(owner_item->isSelected() ? cHighlighted : cDefault);
}

void EntropyChartItem::TreeChart::hoverItem(QTreeWidgetItem *item) {
    if(owner_item->isSelected())
        return;
    if(item == owner_item) {
        if(chart()->theme() == cDefault)
            chart()->setTheme(cHover);
    } else {
        if(chart()->theme() == cHover)
            chart()->setTheme(cDefault);
    }
}
