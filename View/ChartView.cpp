#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "ChartView.h"
#include <QVector>
#include <QPointF>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QToolTip>
#include <QCursor>
#include <QStringList>
#include <QString>
#include <algorithm>
#include <QFont> // 【新增】用于调整字体大小
#include <QGraphicsLayout>

using namespace QtCharts;

ChartView::ChartView(QWidget* pParent)
    : QChartView(pParent), mpMainChart(new QChart()) {
    initializeChart();
}

void ChartView::initializeChart() {
    mpMainChart->setAnimationOptions(QChart::NoAnimation);
    mpMainChart->setTheme(QChart::ChartThemeDark);
    mpMainChart->setTitle("请先载入数据，然后选择图表类型");
    this->setChart(mpMainChart);
    this->setRenderHint(QPainter::Antialiasing);

    // 【问题2 修复核心】：强制允许图表随着 Splitter 自由缩放
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setMinimumSize(100, 100); // 允许无限缩小防止卡死 Splitter
    mpMainChart->layout()->setContentsMargins(0, 0, 0, 0); // 消除多余的图表外白边
}

void ChartView::clearChart() {
    mpMainChart->removeAllSeries();
    auto axesList = mpMainChart->axes();
    for (auto* pAxis : axesList) {
        mpMainChart->removeAxis(pAxis);
    }
}

void ChartView::updateChartData(const std::vector<double>& _vDataPoints, bool _bIsLineChart) {
    if (_vDataPoints.empty()) return;
    clearChart();
    if (_bIsLineChart) {
        drawLineChart(_vDataPoints);
    }
    else {
        drawBarChart(_vDataPoints);
    }
}

void ChartView::drawLineChart(const std::vector<double>& _vDataPoints) {
    QLineSeries* pLineSeries = new QLineSeries();
    QVector<QPointF> _qPoints;

    size_t _nLimit = _vDataPoints.size() > 1000 ? 1000 : _vDataPoints.size();
    qreal _dMinY = _vDataPoints[0];
    qreal _dMaxY = _vDataPoints[0];

    for (size_t i = 0; i < _nLimit; ++i) {
        qreal _dVal = static_cast<qreal>(_vDataPoints[i]);
        _qPoints.append(QPointF(static_cast<qreal>(i), _dVal));
        if (_dVal < _dMinY) _dMinY = _dVal;
        if (_dVal > _dMaxY) _dMaxY = _dVal;
    }

    pLineSeries->replace(_qPoints);
    connect(pLineSeries, &QLineSeries::hovered, this, &ChartView::onLineHoveredSlot);

    mpMainChart->addSeries(pLineSeries);
    mpMainChart->setTitle("数据序列折线图 (仅展示前 1000 个采样点)");

    QValueAxis* pAxisX = new QValueAxis();
    pAxisX->setRange(0, static_cast<qreal>(_nLimit - 1));
    pAxisX->setTitleText("序列索引");

    QValueAxis* pAxisY = new QValueAxis();
    qreal _dPadding = (_dMaxY - _dMinY) * 0.1;
    if (_dPadding <= 0.0) _dPadding = 1.0;
    pAxisY->setRange(_dMinY - _dPadding, _dMaxY + _dPadding);
    pAxisY->setTitleText("数值大小");

    mpMainChart->addAxis(pAxisX, Qt::AlignBottom);
    mpMainChart->addAxis(pAxisY, Qt::AlignLeft);
    pLineSeries->attachAxis(pAxisX);
    pLineSeries->attachAxis(pAxisY);
}

void ChartView::drawBarChart(const std::vector<double>& _vDataPoints) {
    double _dMin = *std::min_element(_vDataPoints.begin(), _vDataPoints.end());
    double _dMax = *std::max_element(_vDataPoints.begin(), _vDataPoints.end());
    if (_dMax <= _dMin) _dMax = _dMin + 1.0;

    // 【问题3 修复核心 1/2】：将 20 个柱子减少为 8 个，防止 X 轴标签重叠被自动隐藏
    int _nBinCount = 8;
    double _dStep = (_dMax - _dMin) / _nBinCount;

    std::vector<int> _vBins(_nBinCount, 0);
    for (double _dVal : _vDataPoints) {
        int _nIdx = static_cast<int>((_dVal - _dMin) / _dStep);
        if (_nIdx >= _nBinCount) _nIdx = _nBinCount - 1;
        if (_nIdx < 0) _nIdx = 0;
        _vBins[_nIdx]++;
    }

    QBarSet* pBarSet = new QBarSet("像元频次");
    pBarSet->setColor(QColor(0, 162, 255));

    QStringList _lstCategories;
    int _nMaxFreq = 0;

    for (int i = 0; i < _nBinCount; ++i) {
        *pBarSet << _vBins[i];
        if (_vBins[i] > _nMaxFreq) _nMaxFreq = _vBins[i];

        // 标签仅保留 1 位小数，尽量简短
        QString _strLabel = QString("%1~%2")
            .arg(_dMin + i * _dStep, 0, 'f', 1)
            .arg(_dMin + (i + 1) * _dStep, 0, 'f', 1);
        _lstCategories << _strLabel;
    }

    QBarSeries* pBarSeries = new QBarSeries();
    pBarSeries->append(pBarSet);
    connect(pBarSeries, &QBarSeries::hovered, this, &ChartView::onBarHoveredSlot);

    mpMainChart->addSeries(pBarSeries);
    mpMainChart->setTitle(QString("全局数据分布直方图 (总数据量: %1)").arg(_vDataPoints.size()));

    QBarCategoryAxis* pAxisX = new QBarCategoryAxis();
    pAxisX->append(_lstCategories);
    pAxisX->setTitleText("数值区间分布");
    pAxisX->setLabelsAngle(-45); // 倾斜 45 度显示

    // 【问题3 修复核心 2/2】：强制缩小 X 轴标签字体为 8 号，保证能塞得下
    QFont labelFont;
    labelFont.setPointSize(8);
    pAxisX->setLabelsFont(labelFont);

    QValueAxis* pAxisY = new QValueAxis();
    pAxisY->setRange(0, _nMaxFreq * 1.1);
    pAxisY->setTitleText("频次");

    mpMainChart->addAxis(pAxisX, Qt::AlignBottom);
    mpMainChart->addAxis(pAxisY, Qt::AlignLeft);
    pBarSeries->attachAxis(pAxisX);
    pBarSeries->attachAxis(pAxisY);
}

void ChartView::onLineHoveredSlot(const QPointF& point, bool state) {
    if (state) {
        QString _strTooltip = QString("索引: %1\n数值: %2").arg(point.x()).arg(point.y());
        QToolTip::showText(QCursor::pos(), _strTooltip, this);
    }
    else {
        QToolTip::hideText();
    }
}

void ChartView::onBarHoveredSlot(bool status, int index, QBarSet* barset) {
    if (status && barset) {
        qreal _dVal = barset->at(index);
        QString _strTooltip = QString("频次: %1").arg(_dVal);
        QToolTip::showText(QCursor::pos(), _strTooltip, this);
    }
    else {
        QToolTip::hideText();
    }
}