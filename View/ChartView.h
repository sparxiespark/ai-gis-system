#ifndef VIEW_CHARTVIEW_H
#define VIEW_CHARTVIEW_H
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSet>

// 确保在头文件中显式使用限定符，避免宏定义失效
class ChartView : public QtCharts::QChartView {
    Q_OBJECT
public:
    explicit ChartView(QWidget* pParent = nullptr);
    ~ChartView() = default;

    void updateChartData(const std::vector<double>& _vDataPoints, bool _bIsLineChart);

private slots:
    void onLineHoveredSlot(const QPointF& point, bool state);
    void onBarHoveredSlot(bool status, int index, QtCharts::QBarSet* barset);

private:
    void initializeChart();
    void clearChart();
    void drawLineChart(const std::vector<double>& _vDataPoints);
    void drawBarChart(const std::vector<double>& _vDataPoints);

    QtCharts::QChart* mpMainChart;
};

#endif
