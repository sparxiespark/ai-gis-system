// service/llm/ResponseParser.h
#ifndef SERVICE_LLM_RESPONSEPARSER_H
#define SERVICE_LLM_RESPONSEPARSER_H

#include <string>

class ResponseParser {
public:
    ResponseParser() = default;
    ~ResponseParser() = default;

    std::string parse(const std::string& _strRawResponse);
};

#endif