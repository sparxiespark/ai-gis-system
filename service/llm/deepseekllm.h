#ifndef SERVICE_LLM_DEEPSEEKLLM_H
#define SERVICE_LLM_DEEPSEEKLLM_H

#include "llminterface.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHash>
#include <QString>

class DeepSeekLLM : public QObject, public LLMInterface {
    Q_OBJECT
public:
    explicit DeepSeekLLM(const QString& _strApiKey, QObject* pParent = nullptr);
    ~DeepSeekLLM() override;

    void query(const std::string& _strPrompt, std::function<void(const std::string&)> _callback) override;

private slots:
    // 【修复核心】：直接在槽函数参数里接收 QNetworkReply 指针，而不是无参
    void onReplyFinished(QNetworkReply* pReply);

private:
    QString m_strApiKey;
    QNetworkAccessManager* m_pNetworkManager;
    QHash<QNetworkReply*, std::function<void(const std::string&)>> m_mapCallbacks;
};

#endif // SERVICE_LLM_DEEPSEEKLLM_H