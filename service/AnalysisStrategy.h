// service/AnalysisStrategy.h
#ifndef SERVICE_ANALYSISSTRATEGY_H_S1T2
#define SERVICE_ANALYSISSTRATEGY_H_S1T2

class DataModel;

class AnalysisStrategy {
public:
    virtual ~AnalysisStrategy() = default;
    virtual void execute(DataModel& _dataModel) = 0;
};
#endif