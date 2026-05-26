#ifndef UI_MAINWINDOW_H_E5F6G7H8_I9J0
#define UI_MAINWINDOW_H_E5F6G7H8_I9J0

#include <QMainWindow>

class QDockWidget;
class QTextEdit;
class QSplitter;
class QStackedWidget;
class QButtonGroup;
class QString;
class QLineEdit;   // 【新增】大模型输入框
class QPushButton; // 【新增】大模型按钮

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* pParent = nullptr);
    ~MainWindow();

    void updateDisplay(const QString& _strMessage);
    void setChartWidget(QWidget* pChartWidget);

    // 【新增】大模型结果回调入口
    void displayLLMResult(const QString& _strResult);

signals:
    // UI 向 Controller 抛出的核心业务信号
    void signalLoadCsv(const QString& _strFilePath);
    void signalLoadRaster(const QString& _strFilePath);
    void signalRunBasicStat();
    void signalRunNeighborStat();
    void signalDrawLineChart();
    void signalDrawBarChart();

    // 【新增】大模型查询指令
    void signalLLMQuery(const QString& _strQuestion);

private slots:
    // 本地 UI 槽函数
    void onTopCategoryChanged(int _nIndex);
    void onBtnLoadCsv();
    void onBtnLoadRaster();
    void onBtnBasicStat();
    void onBtnNeighborStat();
    void onBtnLineChart();
    void onBtnBarChart();

    // 【新增】大模型请求按钮点击事件
    void onBtnLlmClicked();

private:
    void initUi();
    void applyDarkStyleSheet();

private:
    // 第一行：顶部分类按钮组
    QButtonGroup* mpctrlTopButtonGroup;
    // 第二行：动态功能区面板
    QStackedWidget* mpctrlSubMenuStack;

    QDockWidget* mpctrlDockLayerPanel;
    QTextEdit* mpctrlLayerTreeMock;
    QTextEdit* mpctrlCentralMapArea;
    QSplitter* mpctrlCentralSplitter;

    // ================= 【新增】大模型 UI 控件指针 =================
    QDockWidget* m_pDockLlmPanel;
    QLineEdit* m_pLlmInput;
    QPushButton* m_pLlmBtn;
    QTextEdit* m_pLlmOutput;
};

#endif