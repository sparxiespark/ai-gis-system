// service/AnalysisModule.cpp
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "AnalysisModule.h"
#include "AnalysisStrategy.h"
#include "../model/DataModel.h"
#include <QString>

AnalysisModule::AnalysisModule(QObject* pParent) : QObject(pParent) {
    mpStrategy = nullptr;
}

AnalysisModule::~AnalysisModule() {
    if (mpStrategy != nullptr) {
        delete mpStrategy;
        mpStrategy = nullptr;
    }
}

void AnalysisModule::setStrategy(AnalysisStrategy* pStrategy) {
    if (mpStrategy != nullptr) {
        delete mpStrategy;
        mpStrategy = nullptr;
    }
    mpStrategy = pStrategy;
}

void AnalysisModule::executeAnalysis(DataModel* pDataModel) {
    if (mpStrategy != nullptr && pDataModel != nullptr) {
        mpStrategy->execute(*pDataModel);
        emit signalAnalysisDone(pDataModel->getAnalysisResult());
    }
    else {
        emit signalAnalysisDone("[失败] 分析中止：策略未配置或未加载数据！");
    }
}