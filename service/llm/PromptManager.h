// service/llm/PromptManager.h
#ifndef SERVICE_LLM_PROMPTMANAGER_H
#define SERVICE_LLM_PROMPTMANAGER_H

#include <string>

class DataModel;

class PromptManager {
public:
    PromptManager() = default;
    ~PromptManager() = default;

    std::string buildPrompt(const std::string& _strQuestion, const DataModel* _pDataModel);
};

#endif