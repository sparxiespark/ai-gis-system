// service/llm/LLMService.cpp
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "llmservice.h"
#include "llminterface.h"

LLMService::LLMService(LLMInterface* _pModel, const DataModel* _pDataModel)
    : m_pModel(_pModel), m_pDataModel(_pDataModel) {
}

// 【修复核心】：严格使用 void (..., std::function) 签名，取代旧版的 std::string
void LLMService::ask(const std::string& _strQuestion, std::function<void(const std::string&)> _callback) {
    if (!m_pModel) {
        if (_callback) _callback("错误：大模型接口未初始化。");
        return;
    }

    std::string strPrompt = m_promptManager.buildPrompt(_strQuestion, m_pDataModel);

    // 调用底层异步模型，并将解析逻辑注入回调链
    m_pModel->query(strPrompt, [this, _callback](const std::string& _strRawResponse) {
        std::string strResult = m_responseParser.parse(_strRawResponse);
        if (_callback) {
            _callback(strResult);
        }
        });
}