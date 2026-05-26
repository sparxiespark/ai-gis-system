// service/BasicStatStrategy.h
#ifndef SERVICE_BASICSTATSTRATEGY_H
#define SERVICE_BASICSTATSTRATEGY_H
#include "AnalysisStrategy.h"

class BasicStatStrategy : public AnalysisStrategy {
public:
    BasicStatStrategy() = default;
    ~BasicStatStrategy() override = default;
    void execute(DataModel& _dataModel) override;
};
#endif