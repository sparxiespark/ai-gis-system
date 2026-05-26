// service/AnalysisModule.h
#ifndef SERVICE_ANALYSISMODULE_H_C3D4
#define SERVICE_ANALYSISMODULE_H_C3D4

#include <QObject>

class QString;
class AnalysisStrategy;
class DataModel;

class AnalysisModule : public QObject {
    Q_OBJECT
public:
    explicit AnalysisModule(QObject* pParent = nullptr);
    ~AnalysisModule();
    void setStrategy(AnalysisStrategy* pStrategy);
    void executeAnalysis(DataModel* pDataModel);
signals:
    void signalAnalysisDone(const QString& _strResult);
private:
    AnalysisStrategy* mpStrategy;
};
#endif