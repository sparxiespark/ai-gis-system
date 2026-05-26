# GIS 空间分析引擎 (Spatial AnalysisEngine)

这是一个高度模块化、面向工业级标准设计的 GIS 数据分析与可视化系统。系统全面采用 **MVC (Model-View-Controller)** 架构与**策略模式 (Strategy Pattern)**，并以**最小侵入方式**集成了基于 **DeepSeek 大模型的智能 Agent 自动化流编排中枢**。系统原生支持大规模一维空间序列数据、栅格数据的导入、分析、自适应动态图表渲染及智能化交互。

---

## 核心架构设计

系统设计严格遵循面向对象的高内聚、低耦合原则，核心分为五个大层级：

1. **M (Model) 数据层**：`DataModel` 负责常驻内存存储核心 GIS 数据、分辨率及分析结果。为大模型提供**只读上下文**保障，严禁外部直接污染核心数据。
2. **V (View) 视图层**：`MainWindow` 负责复杂的现代暗黑主题 UI 渲染。包含左侧分析控制台、中央大图表/地图回显区，以及右侧独立悬停的 **“智能分析助手”** 面板。
3. **C (Controller) 控制层**：`AppController` 作为全系统的核心神经中枢，不参与具体业务逻辑，专职负责信号的分发、组件的协同以及大模型 JSON 协议指令的自动化调度。
4. **Service (GIS 业务服务层)**：
   * `DataModule`：利用 GDAL/QGIS 接口，高效异步读取 CSV 与 Raster 栅格数据。
   * `AnalysisModule`：通过**策略模式**动态切换 `BasicStatStrategy`（基础统计）与 `NeighborStatStrategy`（邻域分析），具备极强的横向扩展能力。
5. **LLM Service (大模型智能子系统)**：独立于主业务架构之外，位于 `service/llm/` 目录。基于 Qt 的 `QNetworkAccessManager` 实现了高并发、非阻塞的**异步回调架构**。

### 全链路异步数据流拓扑图

```text
[用户输入] ──> MainWindow (发出 signalLLMQuery 信号)
                    │
                    ▼
           AppController ──> LLMService::ask()
                                  │
                                  ▼
                     [PromptManager 组装上下文 JSON 协议]
                                  │
                                  ▼
                     [DeepSeekLLM 异步 HTTPS POST] ──> DeepSeek API
                                                               │
                                                               ▼
[核心驱动] <── AppController 槽函数解析 <── [异步 Lamda 回调] <───┘
   │
   ├───> AnalysisModule (自动切换策略执行空间计算)
   └───> VisualizationManager (自动驱动 ChartView 丝滑缩放与区间自适应)

   目录结构规范
系统源码文件路径全部采用小写规范，类成员变量严格遵循 C++ 匈牙利命名法 (Hungarian Notation)（如 mpMainWindow, m_pLlmService, _strQuestion），结构清晰：
├── core/
│   ├── AppController.h          # 核心控制器声明（持有 LLMService 与核心 GIS 模块）
│   └── AppController.cpp        # 负责连接核心信号槽，解析大模型指令进行二级路由分发
├── model/
│   ├── DataModel.h              # 空间分析数据中心（只读上下文提供者）
│   └── DataModel.cpp
├── service/
│   ├── DataModule.h/cpp         # GIS 数据 IO 模块（CSV/栅格读取）
│   ├── AnalysisModule.h/cpp     # 空间分析上下文管理器
│   ├── AnalysisStrategy.h       # 空间分析抽象策略接口
│   ├── BasicStatStrategy.h/cpp  # 基础统计具体策略
│   ├── NeighborStatStrategy.h/cpp# 邻域分析具体策略
│   └── llm/                     # ── 大模型智能集成服务子系统 ──
│       ├── LLMInterface.h       # 大模型抽象策略接口（定义异步回调 query 签名）
│       ├── MockLLM.h/cpp        # 本地流编排自动化模拟器（无网环境测试）
│       ├── DeepSeekLLM.h/cpp    # DeepSeek 官方 API 异步通信实现类（格式约束）
│       ├── PromptManager.h/cpp  # 动态 GIS 上下文提示词拼接器
│       ├── ResponseParser.h/cpp # Markdown 代码块清洗与 JSON 提取器
│       ├── LLMService.h         # 大模型高层聚合服务类（对外核心接口）
│       └── LLMService.cpp       # 串联提示词组装、网络请求与数据解析的回调链
├── ui/
│   ├── MainWindow.h             # 停靠式主窗体控制中心
│   └── MainWindow.cpp           # 增量集成“智能分析助手”Dock 面板，QSS 暗黑皮肤
└── view/
    ├── ChartView.h              # 继承自 QtCharts::QChartView 的自适应画布
    └── ChartView.cpp            # 显式使用 QtCharts 命名空间，修复缩放卡死及标签隐藏痛点

    大模型 Agent 自动化流编排协议
为了让 C++ 核心层精准掌控大模型的动作，PromptManager 强行约束 DeepSeek API 必须返回标准的 json_object。系统内部通信协议定义如下：
{
  "reply": "文字回复内容。如果用户要求进行数据总结，详细的空间数据分析报告也将在此输出。",
  "command": "动作指令类型。可选值：'Analysis' (触发空间计算), 'Visualize' (触发图表转换), 'Chat' (纯文本对话)",
  "target": "动作执行目标。可选值：'BasicStat' (基础统计), 'NeighborStat' (邻域分析), 'LineChart' (折线图), 'BarChart' (直方分布图), 'None'"
}
C++ 调度枢纽拦截示例
当用户在界面输入：“帮我切换成直方图，然后把数据做一个邻域统计”。
DeepSeek 会输出上述标准的 JSON，AppController::handleLLMQuery 拦截后会自发调用：
环境依赖与编译指南
1. 外部依赖库
系统编译需要预先安装并配置好以下开发环境：

C++ 标准：C++17 及以上

编译器：MSVC 2022 / Clang

Qt 工具链：Qt 5.15+ (必须包含 Charts, Network, Widgets 模块)

GIS 核心库：OSGeo4W 环境 (GDAL 3.x, QGIS LTR)

辅助日志与解析：Log4cpp-1.1.4, LibXml2

2. CMake 关键配置
本工程的 CMakeLists.txt 已内置对 MSVC 环境中文编码乱码问题的终极解决方案，自动注入了 /utf-8 编译选项。

请确保在 CMakeLists.txt 顶部正确配置你的本地第三方库存放路径：
set(OSGEO4W_ROOT "D:/cpp/OSGeo4W" CACHE PATH "OSGeo4W 安装根目录")
set(LOG4CPP_ROOT "D:/cpp/log4cpp-1.1.4" CACHE PATH "log4cpp 安装根目录")
set(LIBXML2_ROOT "D:/cpp/libxml2" CACHE PATH "libxml2 安装根目录")

运行与智能交互测试指南
1. 运行前准备
由于项目依赖大量动态链接库（DLL），在 Visual Studio 中调试时，工程已自动配置了 VS_DEBUGGER_ENVIRONMENT 环境变量。若直接双击运行 .exe，请确保将 D:/cpp/OSGeo4W/apps/gdal-dev/bin、D:/cpp/OSGeo4W/apps/Qt5/bin 等路径加入系统环境变量 PATH 中。

2. 填写 API Key
打开 core/AppController.cpp，在构造函数中将您的 DeepSeek 官方 API 密钥写入：