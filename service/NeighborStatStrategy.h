// service/NeighborStatStrategy.h
#ifndef SERVICE_NEIGHBORSTATSTRATEGY_H
#define SERVICE_NEIGHBORSTATSTRATEGY_H
#include "AnalysisStrategy.h"

class NeighborStatStrategy : public AnalysisStrategy {
public:
    NeighborStatStrategy() = default;
    ~NeighborStatStrategy() override = default;
    void execute(DataModel& _dataModel) override;
};
#endif