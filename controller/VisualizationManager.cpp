#include "VisualizationManager.h"
#include "../view/ChartView.h"

VisualizationManager::VisualizationManager(QObject* pParent)
    : QObject(pParent), m_eCurrentType(LineChart) {
    mpChartView = new ChartView();
}

VisualizationManager::~VisualizationManager() {
    if (mpChartView) {
        delete mpChartView;
        mpChartView = nullptr;
    }
}

ChartView* VisualizationManager::getView() const {
    return mpChartView;
}

void VisualizationManager::setChartType(ChartType _eType) {
    m_eCurrentType = _eType;
    // 切换类型后，如果内存中有数据，则立刻重新渲染
    if (!m_vCurrentData.empty()) {
        renderData(m_vCurrentData);
    }
}

void VisualizationManager::renderData(const std::vector<double>& _vData) {
    m_vCurrentData = _vData; // 缓存数据
    if (mpChartView) {
        mpChartView->updateChartData(_vData, m_eCurrentType == LineChart);
    }
}