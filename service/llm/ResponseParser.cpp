// service/llm/ResponseParser.cpp
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "responseparser.h"

std::string ResponseParser::parse(const std::string& _strRawResponse) {
    std::string strClean = _strRawResponse;

    // 【正则化处理】：剥离大模型可能带有的 Markdown 标记 (```json 和 ```)
    size_t nStart = strClean.find("```json");
    if (nStart != std::string::npos) {
        nStart += 7; // 跳过 "```json"
        size_t nEnd = strClean.find("```", nStart);
        if (nEnd != std::string::npos) {
            strClean = strClean.substr(nStart, nEnd - nStart);
        }
    }

    // 如果存在 MockLLM 的旧前缀，一并清除
    const std::string strPrefix = "【LLM分析】";
    if (strClean.find(strPrefix) == 0) {
        strClean = strClean.substr(strPrefix.length());
    }

    return strClean;
}