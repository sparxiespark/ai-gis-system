// service/FreqStatStrategy.h
#ifndef SERVICE_FREQSTATSTRATEGY_H
#define SERVICE_FREQSTATSTRATEGY_H
#include "AnalysisStrategy.h"

class FreqStatStrategy : public AnalysisStrategy {
public:
    FreqStatStrategy() = default;
    ~FreqStatStrategy() override = default;
    void execute(DataModel& _dataModel) override;
};
#endif