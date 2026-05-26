#ifndef SERVICE_LLM_LLMINTERFACE_H
#define SERVICE_LLM_LLMINTERFACE_H

#include <string>
#include <functional> // 新增

class LLMInterface {
public:
    virtual ~LLMInterface() = default;

    // 【修改】：改为支持异步回调的接口签名
    virtual void query(const std::string& _strPrompt, std::function<void(const std::string&)> _callback) = 0;
};

#endif