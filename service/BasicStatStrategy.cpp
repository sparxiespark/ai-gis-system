// service/BasicStatStrategy.cpp
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "BasicStatStrategy.h"
#include "../model/DataModel.h"
#include <QString>
#include <algorithm>
#include <numeric>

void BasicStatStrategy::execute(DataModel& _dataModel) {
    const std::vector<double>& _vValues = _dataModel.getValues();
    if (_vValues.empty()) {
        _dataModel.setAnalysisResult("[失败] 数据模型为空或无有效数值。");
        return;
    }

    double _dMin = *std::min_element(_vValues.begin(), _vValues.end());
    double _dMax = *std::max_element(_vValues.begin(), _vValues.end());
    double _dSum = std::accumulate(_vValues.begin(), _vValues.end(), 0.0);
    double _dMean = _dSum / _vValues.size();

    QString _strRes = QString("[基础统计分析结果]\n数据类型: %1\n有效数据量: %2 \n最小值: %3\n最大值: %4\n平均值: %5")
        .arg(_dataModel.getDataType() == DataModel::DataType_CSV ? "CSV" : "Raster")
        .arg(_vValues.size()).arg(_dMin).arg(_dMax).arg(_dMean);
    _dataModel.setAnalysisResult(_strRes);
}