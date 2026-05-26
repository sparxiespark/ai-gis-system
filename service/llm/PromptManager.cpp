#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "promptmanager.h"
#include "../../model/DataModel.h" 

std::string PromptManager::buildPrompt(const std::string& _strQuestion, const DataModel* _pDataModel) {
    std::string strContext = "暂无数据";
    if (_pDataModel != nullptr) {
        strContext = _pDataModel->getAnalysisResult().toStdString();
        // 如果用户没点分析，告诉模型实情
        if (strContext.empty()) strContext = "【注意】：当前用户仅载入了数据，尚未执行基础统计或邻域分析，因此无统计数据可供总结。请提示用户先执行统计。";
    }

    // 【问题1 修复核心】：在 Prompt 中明确授权模型，当遇到总结需求时，将内容写在 reply 中
    std::string strProtocol =
        "你是一个 GIS 空间分析引擎的智能中枢。请分析用户指令，并【必须】严格以 JSON 格式输出，不要包含任何 Markdown 代码块外的废话。\n"
        "你可以执行的动作如下：\n"
        "1. 分析指令：command=\"Analysis\", target 可选: \"BasicStat\"(基础统计), \"NeighborStat\"(邻域分析)\n"
        "2. 可视化指令：command=\"Visualize\", target 可选: \"LineChart\"(折线图), \"BarChart\"(柱状图)\n"
        "3. 日常对话与数据总结：command=\"Chat\", target=\"None\"\n"
        "\n【特别指令】：如果用户要求【总结】数据特征，请仔细阅读下方的'当前数据上下文'，并在 JSON 的 `reply` 字段中输出一段详细、专业的中文总结报告！\n"
        "\n请返回如下 JSON 结构：\n"
        "{\n"
        "  \"reply\": \"你想对用户说的话，或者是详细的中文数据特征总结报告。\",\n"
        "  \"command\": \"对应的指令\",\n"
        "  \"target\": \"对应的目标\"\n"
        "}\n";

    std::string strFinalPrompt = strProtocol + "\n当前数据上下文：\n" + strContext + "\n用户输入：" + _strQuestion;
    return strFinalPrompt;
}