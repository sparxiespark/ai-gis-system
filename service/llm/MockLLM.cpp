// service/llm/MockLLM.cpp
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "mockllm.h"

void MockLLM::query(const std::string& _strPrompt, std::function<void(const std::string&)> _callback) {
    std::string strResponse;

    if (_strPrompt.find("邻域") != std::string::npos) {
        strResponse = R"({"reply": "正在执行邻域分析...", "command": "Analysis", "target": "NeighborStat"})";
    }
    else {
        strResponse = R"({"reply": "我是一个本地模拟器。", "command": "Chat", "target": "None"})";
    }

    // 模拟立刻回调
    if (_callback) _callback(strResponse);
}