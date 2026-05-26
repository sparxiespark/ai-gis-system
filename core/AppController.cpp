#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "AppController.h"
#include "../ui/MainWindow.h"
#include "../service/DataModule.h"
#include "../service/AnalysisModule.h"
#include "../model/DataModel.h"
#include "../service/BasicStatStrategy.h"
#include "../service/NeighborStatStrategy.h"
#include "../controller/VisualizationManager.h"
#include "../view/ChartView.h" // 【修复核心】：必须包含这个头文件，否则 ChartView* 无法转为 QWidget*

// 引入大模型模块
#include "../service/llm/deepseekllm.h" 
#include "../service/llm/mockllm.h"
#include "../service/llm/llmservice.h"

// 引入 Qt JSON 解析
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

AppController::AppController(QObject* pParent) : QObject(pParent) {
    mpMainWindow = new MainWindow();
    mpDataModule = new DataModule(this);
    mpAnalysisModule = new AnalysisModule(this);
    mpVisualManager = new VisualizationManager(this);
    mpDataModel = new DataModel(this);

    // 注意：在这里填入您的 API Key
    m_pLlmModel = new DeepSeekLLM("sk-114514", this);

    m_pLlmService = new LLMService(m_pLlmModel, mpDataModel);

    // 修复点：现在编译器知道 mpVisualManager->getView() 返回的 ChartView* 是 QWidget 的子类了
    mpMainWindow->setChartWidget(mpVisualManager->getView());

    connect(mpDataModel, &DataModel::signalDataUpdated, this, [this](const std::vector<double>& _vData) {
        if (mpVisualManager) mpVisualManager->renderData(_vData);
        });

    connect(mpMainWindow, &MainWindow::signalLoadCsv, this, &AppController::onUiLoadCsv);
    connect(mpMainWindow, &MainWindow::signalLoadRaster, this, &AppController::onUiLoadRaster);
    connect(mpMainWindow, &MainWindow::signalRunBasicStat, this, &AppController::onUiRunBasicStat);
    connect(mpMainWindow, &MainWindow::signalRunNeighborStat, this, &AppController::onUiRunNeighborStat);

    connect(mpMainWindow, &MainWindow::signalDrawLineChart, this, [this]() {
        if (mpVisualManager) mpVisualManager->setChartType(VisualizationManager::LineChart);
        });
    connect(mpMainWindow, &MainWindow::signalDrawBarChart, this, [this]() {
        if (mpVisualManager) mpVisualManager->setChartType(VisualizationManager::BarChart);
        });

    connect(mpAnalysisModule, &AnalysisModule::signalAnalysisDone, this, &AppController::onAnalysisCompleted);

    // 连接 LLM 信号
    connect(mpMainWindow, &MainWindow::signalLLMQuery, this, &AppController::handleLLMQuery);
}

AppController::~AppController() {
    if (m_pLlmService) delete m_pLlmService;
    if (mpMainWindow) delete mpMainWindow;
}

void AppController::initialize() {
    if (mpMainWindow) mpMainWindow->show();
}

void AppController::onUiLoadCsv(const QString& _strFilePath) {
    if (_strFilePath.isEmpty()) return;
    mpDataModule->readCsvData(_strFilePath, mpDataModel);
    mpMainWindow->updateDisplay(QString("[成功] CSV 加载完成！\n数据点数: %1").arg(mpDataModel->getValues().size()));
}

void AppController::onUiLoadRaster(const QString& _strFilePath) {
    if (_strFilePath.isEmpty()) return;
    mpDataModule->readRasterData(_strFilePath, mpDataModel);
    mpMainWindow->updateDisplay(QString("[成功] 栅格加载完成！\n像元点数: %1").arg(mpDataModel->getValues().size()));
}

void AppController::onUiRunBasicStat() {
    if (mpDataModel->getValues().empty()) return;
    mpAnalysisModule->setStrategy(new BasicStatStrategy());
    mpAnalysisModule->executeAnalysis(mpDataModel);
}

void AppController::onUiRunNeighborStat() {
    if (mpDataModel->getValues().empty()) return;
    mpAnalysisModule->setStrategy(new NeighborStatStrategy());
    mpAnalysisModule->executeAnalysis(mpDataModel);
}

void AppController::onAnalysisCompleted(const QString& _strResult) {
    mpMainWindow->updateDisplay(_strResult);
}

// 自动调度中枢：异步处理 + JSON 解析流编排
void AppController::handleLLMQuery(const QString& _strQuestion) {
    if (!m_pLlmService) return;

    mpMainWindow->displayLLMResult("<i>正在请求 DeepSeek 处理指令，请稍候...</i>");

    m_pLlmService->ask(_strQuestion.toStdString(), [this](const std::string& _strAnswer) {
        QString strJson = QString::fromStdString(_strAnswer);

        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(strJson.toUtf8(), &jsonError);

        if (jsonError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
            mpMainWindow->displayLLMResult(strJson);
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        QString strReply = jsonObj.value("reply").toString();
        QString strCommand = jsonObj.value("command").toString();
        QString strTarget = jsonObj.value("target").toString();

        if (!strReply.isEmpty()) {
            mpMainWindow->displayLLMResult(strReply);
        }

        if (strCommand == "Analysis") {
            if (strTarget == "BasicStat") {
                onUiRunBasicStat();
            }
            else if (strTarget == "NeighborStat") {
                onUiRunNeighborStat();
            }
        }
        else if (strCommand == "Visualize") {
            if (strTarget == "LineChart") {
                if (mpVisualManager) mpVisualManager->setChartType(VisualizationManager::LineChart);
            }
            else if (strTarget == "BarChart") {
                if (mpVisualManager) mpVisualManager->setChartType(VisualizationManager::BarChart);
            }
        }
        });
}