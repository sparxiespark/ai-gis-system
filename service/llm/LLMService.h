// service/llm/LLMService.h
#ifndef SERVICE_LLM_LLMSERVICE_H
#define SERVICE_LLM_LLMSERVICE_H

#include <string>
#include <functional>
#include "promptmanager.h"
#include "responseparser.h"

class LLMInterface;
class DataModel;

class LLMService {
public:
    LLMService(LLMInterface* _pModel, const DataModel* _pDataModel);
    ~LLMService() = default;

    // 改为异步询问
    void ask(const std::string& _strQuestion, std::function<void(const std::string&)> _callback);

private:
    LLMInterface* m_pModel;
    const DataModel* m_pDataModel;

    PromptManager m_promptManager;
    ResponseParser m_responseParser;
};

#endif