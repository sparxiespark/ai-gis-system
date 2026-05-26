#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "MainWindow.h"
#include <QDockWidget>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QPushButton>
#include <QFileDialog>
#include <QString>
#include <QWidget>
#include <QLineEdit> // 【新增】大模型需要输入框

MainWindow::MainWindow(QWidget* pParent) : QMainWindow(pParent) {
    initUi();
    applyDarkStyleSheet();
    resize(1200, 800);
    setWindowTitle("GIS 空间分析引擎 - 选项卡导航版");
}

MainWindow::~MainWindow() {}

void MainWindow::initUi() {
    QWidget* pCentralWidget = new QWidget(this);
    QVBoxLayout* pMainLayout = new QVBoxLayout(pCentralWidget);
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    pMainLayout->setSpacing(0);

    // ==================== 第一行：顶部分类导航 ====================
    QWidget* pTopRow = new QWidget(this);
    pTopRow->setObjectName("TopRow");
    QHBoxLayout* pTopLayout = new QHBoxLayout(pTopRow);
    pTopLayout->setContentsMargins(10, 5, 10, 0);
    pTopLayout->setSpacing(0);

    mpctrlTopButtonGroup = new QButtonGroup(this);
    mpctrlTopButtonGroup->setExclusive(true);

    QPushButton* pBtnData = new QPushButton("导入数据", this);
    QPushButton* pBtnAnalysis = new QPushButton("数据分析", this);
    QPushButton* pBtnVis = new QPushButton("可视化", this);

    pBtnData->setCheckable(true);
    pBtnAnalysis->setCheckable(true);
    pBtnVis->setCheckable(true);

    mpctrlTopButtonGroup->addButton(pBtnData, 0);
    mpctrlTopButtonGroup->addButton(pBtnAnalysis, 1);
    mpctrlTopButtonGroup->addButton(pBtnVis, 2);

    pTopLayout->addWidget(pBtnData);
    pTopLayout->addWidget(pBtnAnalysis);
    pTopLayout->addWidget(pBtnVis);
    pTopLayout->addStretch();

    // ==================== 第二行：动态子菜单 ====================
    mpctrlSubMenuStack = new QStackedWidget(this);
    mpctrlSubMenuStack->setObjectName("SubMenuRow");

    // Page 0: 导入数据子面板
    QWidget* pPageData = new QWidget();
    QHBoxLayout* pPageDataLayout = new QHBoxLayout(pPageData);
    pPageDataLayout->setContentsMargins(10, 5, 10, 5);
    QPushButton* pBtnLoadCsv = new QPushButton("导入 CSV", this);
    QPushButton* pBtnLoadRaster = new QPushButton("导入 Raster", this);
    pPageDataLayout->addWidget(pBtnLoadCsv);
    pPageDataLayout->addWidget(pBtnLoadRaster);
    pPageDataLayout->addStretch();
    mpctrlSubMenuStack->addWidget(pPageData);

    // Page 1: 数据分析子面板
    QWidget* pPageAnalysis = new QWidget();
    QHBoxLayout* pPageAnalysisLayout = new QHBoxLayout(pPageAnalysis);
    pPageAnalysisLayout->setContentsMargins(10, 5, 10, 5);
    QPushButton* pBtnBasicStat = new QPushButton("基础统计", this);
    QPushButton* pBtnNeighborStat = new QPushButton("邻域分析", this);
    pPageAnalysisLayout->addWidget(pBtnBasicStat);
    pPageAnalysisLayout->addWidget(pBtnNeighborStat);
    pPageAnalysisLayout->addStretch();
    mpctrlSubMenuStack->addWidget(pPageAnalysis);

    // Page 2: 可视化子面板
    QWidget* pPageVis = new QWidget();
    QHBoxLayout* pPageVisLayout = new QHBoxLayout(pPageVis);
    pPageVisLayout->setContentsMargins(10, 5, 10, 5);
    QPushButton* pBtnLineChart = new QPushButton("折线图", this);
    QPushButton* pBtnBarChart = new QPushButton("柱状图", this);
    pPageVisLayout->addWidget(pBtnLineChart);
    pPageVisLayout->addWidget(pBtnBarChart);
    pPageVisLayout->addStretch();
    mpctrlSubMenuStack->addWidget(pPageVis);

    pMainLayout->addWidget(pTopRow);
    pMainLayout->addWidget(mpctrlSubMenuStack);

    // ==================== 核心显示区域 ====================
    mpctrlCentralSplitter = new QSplitter(Qt::Vertical, this);
    mpctrlCentralMapArea = new QTextEdit(this);
    mpctrlCentralMapArea->setReadOnly(true);
    mpctrlCentralMapArea->setAlignment(Qt::AlignCenter);
    mpctrlCentralMapArea->setText("文本回显视图\n(请点击上方工具栏测试)");

    mpctrlCentralSplitter->addWidget(mpctrlCentralMapArea);
    pMainLayout->addWidget(mpctrlCentralSplitter, 1);
    setCentralWidget(pCentralWidget);

    // 侧边栏
    mpctrlDockLayerPanel = new QDockWidget("分析控制台", this);
    mpctrlLayerTreeMock = new QTextEdit(mpctrlDockLayerPanel);
    mpctrlLayerTreeMock->setReadOnly(true);
    mpctrlLayerTreeMock->setText(" 当前项目尚未分配数据。\n\n 操作步骤：\n 1. 导入数据\n 2. 可视化\n 3. 数据分析");
    mpctrlDockLayerPanel->setWidget(mpctrlLayerTreeMock);
    addDockWidget(Qt::LeftDockWidgetArea, mpctrlDockLayerPanel);

    // ==================== 【新增】右侧大模型面板 ====================
    m_pDockLlmPanel = new QDockWidget("智能分析助手", this);
    QWidget* pLlmWidget = new QWidget(m_pDockLlmPanel);
    QVBoxLayout* pLlmLayout = new QVBoxLayout(pLlmWidget);

    m_pLlmOutput = new QTextEdit(pLlmWidget);
    m_pLlmOutput->setReadOnly(true);
    m_pLlmOutput->setPlaceholderText("在此等待模型响应...");

    QHBoxLayout* pLlmInputLayout = new QHBoxLayout();
    m_pLlmInput = new QLineEdit(pLlmWidget);
    m_pLlmInput->setPlaceholderText("请输入针对当前数据的提问...");
    m_pLlmBtn = new QPushButton("智能分析", pLlmWidget);

    pLlmInputLayout->addWidget(m_pLlmInput);
    pLlmInputLayout->addWidget(m_pLlmBtn);

    pLlmLayout->addWidget(m_pLlmOutput);
    pLlmLayout->addLayout(pLlmInputLayout);

    m_pDockLlmPanel->setWidget(pLlmWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_pDockLlmPanel); // 将面板停靠在主窗口右侧

    // ==================== 信号绑定 ====================
    connect(mpctrlTopButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
        this, &MainWindow::onTopCategoryChanged);

    connect(pBtnLoadCsv, &QPushButton::clicked, this, &MainWindow::onBtnLoadCsv);
    connect(pBtnLoadRaster, &QPushButton::clicked, this, &MainWindow::onBtnLoadRaster);
    connect(pBtnBasicStat, &QPushButton::clicked, this, &MainWindow::onBtnBasicStat);
    connect(pBtnNeighborStat, &QPushButton::clicked, this, &MainWindow::onBtnNeighborStat);
    connect(pBtnLineChart, &QPushButton::clicked, this, &MainWindow::onBtnLineChart);
    connect(pBtnBarChart, &QPushButton::clicked, this, &MainWindow::onBtnBarChart);

    // 【新增】智能分析按钮事件
    connect(m_pLlmBtn, &QPushButton::clicked, this, &MainWindow::onBtnLlmClicked);

    // 初始化默认选中第一项
    pBtnData->setChecked(true);
    mpctrlSubMenuStack->setCurrentIndex(0);
}

void MainWindow::onTopCategoryChanged(int _nIndex) {
    mpctrlSubMenuStack->setCurrentIndex(_nIndex);
}

void MainWindow::onBtnLoadCsv() {
    QString _strPath = QFileDialog::getOpenFileName(this, "选择 CSV 文件", "", "CSV Files (*.csv);;All Files (*)");
    emit signalLoadCsv(_strPath);
}
void MainWindow::onBtnLoadRaster() {
    QString _strPath = QFileDialog::getOpenFileName(this, "选择栅格文件", "", "Raster Files (*.tif *.asc);;All Files (*)");
    emit signalLoadRaster(_strPath);
}
void MainWindow::onBtnBasicStat() { emit signalRunBasicStat(); }
void MainWindow::onBtnNeighborStat() { emit signalRunNeighborStat(); }
void MainWindow::onBtnLineChart() { emit signalDrawLineChart(); }
void MainWindow::onBtnBarChart() { emit signalDrawBarChart(); }

void MainWindow::updateDisplay(const QString& _strMessage) {
    mpctrlLayerTreeMock->setText(" [最新反馈]：\n\n " + _strMessage);
    mpctrlCentralMapArea->setText(_strMessage);
}

void MainWindow::setChartWidget(QWidget* pChartWidget) {
    if (mpctrlCentralSplitter && pChartWidget) {
        mpctrlCentralSplitter->addWidget(pChartWidget);
        mpctrlCentralSplitter->setSizes({ 200, 600 });
    }
}

// ==================== 【新增】大模型槽函数实现 ====================
void MainWindow::onBtnLlmClicked() {
    QString strQuestion = m_pLlmInput->text();
    if (!strQuestion.isEmpty()) {
        // 在对话框里显示用户的提问
        m_pLlmOutput->append("<span style='color:#00A2FF'><b>用户:</b></span> " + strQuestion);
        m_pLlmInput->clear();

        // 发射信号，让 AppController 的 handleLLMQuery 去接管处理
        emit signalLLMQuery(strQuestion);
    }
}

void MainWindow::displayLLMResult(const QString& _strResult) {
    // 将 Controller 传回来的大模型结果显示到对话框中
    m_pLlmOutput->append("<span style='color:#4CAF50'><b>LLM:</b></span> " + _strResult + "<br>");
}

void MainWindow::applyDarkStyleSheet() {
    // 增加了 QLineEdit 的样式
    QString _strStyle = R"(
        QMainWindow, QWidget { background-color: #1E1E1E; color: #E0E0E0; font-family: "Microsoft YaHei"; }
        #TopRow { background-color: #2D2D30; border-bottom: 1px solid #1E1E1E; }
        #SubMenuRow { background-color: #3E3E42; border-bottom: 1px solid #007ACC;}
        QPushButton { 
            background-color: transparent; border: none; padding: 10px 20px; font-size: 14px; 
        }
        #TopRow QPushButton:hover { background-color: #3E3E42; }
        #TopRow QPushButton:checked { 
            background-color: #3E3E42; color: #00A2FF; font-weight: bold;
            border-bottom: 3px solid #00A2FF; 
        }
        #SubMenuRow QPushButton { border-radius: 4px; border: 1px solid #555555; margin-right: 5px; }
        #SubMenuRow QPushButton:hover { background-color: #505050; border: 1px solid #00A2FF; }
        QDockWidget::title { background-color: #2D2D30; padding: 5px; }
        QTextEdit { font-size: 15px; background-color: #252526; border: 1px solid #333333; padding: 10px; }
        QSplitter::handle { background-color: #3E3E42; }
        QLineEdit { background-color: #333337; border: 1px solid #555555; padding: 5px; color: #E0E0E0; border-radius: 2px;}
        QLineEdit:focus { border: 1px solid #007ACC; }
    )";
    setStyleSheet(_strStyle);
}