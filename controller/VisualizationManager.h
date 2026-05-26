#ifndef CONTROLLER_VISUALIZATIONMANAGER_H
#define CONTROLLER_VISUALIZATIONMANAGER_H

#include <QObject>
#include <vector>

class ChartView;
class QWidget;

class VisualizationManager : public QObject {
    Q_OBJECT
public:
    // 图表枚举类型
    enum ChartType { LineChart, BarChart };

    explicit VisualizationManager(QObject* pParent = nullptr);
    ~VisualizationManager();

    ChartView* getView() const;
    void setChartType(ChartType _eType);
    void renderData(const std::vector<double>& _vData);

private:
    ChartView* mpChartView;
    ChartType m_eCurrentType;
    std::vector<double> m_vCurrentData;
};

#endif