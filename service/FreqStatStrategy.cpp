// service/FreqStatStrategy.cpp
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "FreqStatStrategy.h"
#include "../model/DataModel.h"
#include <QString>
#include <map>

void FreqStatStrategy::execute(DataModel& _dataModel) {
    const std::vector<double>& _vValues = _dataModel.getValues();
    if (_vValues.empty()) {
        _dataModel.setAnalysisResult("[失败] 数据模型为空。");
        return;
    }

    std::map<int, int> _mapFreq;
    for (double _dVal : _vValues) { _mapFreq[static_cast<int>(_dVal)]++; }

    QString _strRes = "[频率统计/直方图分析]\n数值区间分布：\n";
    for (auto it = _mapFreq.begin(); it != _mapFreq.end(); ++it) {
        _strRes += QString("区间 [%1, %2) : 频次 %3\n").arg(it->first).arg(it->first + 1).arg(it->second);
    }
    _dataModel.setAnalysisResult(_strRes);
}