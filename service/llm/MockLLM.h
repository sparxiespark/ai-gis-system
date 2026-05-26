// service/llm/MockLLM.h
#ifndef SERVICE_LLM_MOCKLLM_H
#define SERVICE_LLM_MOCKLLM_H

#include "llminterface.h"

class MockLLM : public LLMInterface {
public:
    MockLLM() = default;
    ~MockLLM() override = default;

    void query(const std::string& _strPrompt, std::function<void(const std::string&)> _callback) override;
};

#endif