/// service/NeighborStatStrategy.cpp
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "NeighborStatStrategy.h"
#include "../model/DataModel.h"
#include <QString>
#include <vector>

void NeighborStatStrategy::execute(DataModel& _dataModel) {
    if (_dataModel.getDataType() != DataModel::DataType_Raster) {
        _dataModel.setAnalysisResult("[失败] 邻域分析必须针对 Raster 栅格数据执行！");
        return;
    }

    int _nWidth = _dataModel.getWidth();
    int _nHeight = _dataModel.getHeight();
    const std::vector<double>& _vValues = _dataModel.getValues();

    if (_nWidth < 3 || _nHeight < 3 || _vValues.empty()) {
        _dataModel.setAnalysisResult("[失败] 栅格尺寸过小，无法执行 3x3 滑动窗口。");
        return;
    }

    int _nProcessedCount = 0;
    double _dSumFiltered = 0.0;

    for (int y = 1; y < _nHeight - 1; ++y) {
        for (int x = 1; x < _nWidth - 1; ++x) {
            double _dLocalSum = 0.0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    _dLocalSum += _vValues[(y + dy) * _nWidth + (x + dx)];
                }
            }
            _dSumFiltered += (_dLocalSum / 9.0);
            _nProcessedCount++;
        }
    }

    double _dGlobalMean = _dSumFiltered / _nProcessedCount;
    QString _strRes = QString("[3x3 邻域平滑分析完成]\n参与计算像元数: %1\n平滑后全图均值: %2")
        .arg(_nProcessedCount).arg(_dGlobalMean);
    _dataModel.setAnalysisResult(_strRes);
}