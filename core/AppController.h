#ifndef CORE_APPCONTROLLER_H_B1C2
#define CORE_APPCONTROLLER_H_B1C2

#include <QObject>

class MainWindow;
class DataModule;
class AnalysisModule;
class DataModel;
class VisualizationManager;
class QString;
class LLMInterface;
class LLMService;

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* pParent = nullptr);
    ~AppController();
    void initialize();

private slots:
    void onUiLoadCsv(const QString& _strFilePath);
    void onUiLoadRaster(const QString& _strFilePath);
    void onUiRunBasicStat();
    void onUiRunNeighborStat();
    void onAnalysisCompleted(const QString& _strResult);

    // 【新增】处理大模型请求
    void handleLLMQuery(const QString& _strQuestion);

private:
    MainWindow* mpMainWindow;
    DataModule* mpDataModule;
    AnalysisModule* mpAnalysisModule;
    VisualizationManager* mpVisualManager;
    DataModel* mpDataModel;

    // 【新增】
    LLMInterface* m_pLlmModel;
    LLMService* m_pLlmService;
};
#endif