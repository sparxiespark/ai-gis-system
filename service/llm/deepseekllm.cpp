#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "deepseekllm.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

DeepSeekLLM::DeepSeekLLM(const QString& _strApiKey, QObject* pParent)
    : QObject(pParent), m_strApiKey(_strApiKey) {
    m_pNetworkManager = new QNetworkAccessManager(this);

    // 绑定信号到有参槽函数
    connect(m_pNetworkManager, &QNetworkAccessManager::finished, this, &DeepSeekLLM::onReplyFinished);
}

DeepSeekLLM::~DeepSeekLLM() {
}

void DeepSeekLLM::query(const std::string& _strPrompt, std::function<void(const std::string&)> _callback) {
    QUrl url("https://api.deepseek.com/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString strAuth = "Bearer " + m_strApiKey;
    request.setRawHeader("Authorization", strAuth.toUtf8());

    QJsonObject msg;
    msg["role"] = "user";
    msg["content"] = QString::fromStdString(_strPrompt);

    QJsonArray messages;
    messages.append(msg);

    QJsonObject body;
    body["model"] = "deepseek-chat";
    body["messages"] = messages;

    QJsonObject format;
    format["type"] = "json_object";
    body["response_format"] = format;

    QJsonDocument doc(body);
    QByteArray data = doc.toJson();

    QNetworkReply* pReply = m_pNetworkManager->post(request, data);

    // 将指针及其回调函数存入映射表
    m_mapCallbacks.insert(pReply, _callback);
}

// 【修复核心】：直接使用系统传入的 pReply
void DeepSeekLLM::onReplyFinished(QNetworkReply* pReply) {
    if (!pReply) return;

    std::string strResponse = "";
    // 从映射表中取出对应的回调
    auto callback = m_mapCallbacks.value(pReply);

    // 解析返回值
    if (pReply->error() == QNetworkReply::NoError) {
        QByteArray responseData = pReply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        QJsonObject obj = doc.object();
        QJsonArray choices = obj["choices"].toArray();
        if (!choices.isEmpty()) {
            QJsonObject msg = choices[0].toObject()["message"].toObject();
            strResponse = msg["content"].toString().toStdString();
        }
    }
    else {
        strResponse = "{\"reply\": \"网络请求失败: " + pReply->errorString().toStdString() + "\", \"command\": \"Chat\", \"target\": \"None\"}";
    }

    // 执行回调交还给 AppController
    if (callback) {
        callback(strResponse);
    }

    // 安全清理内存
    m_mapCallbacks.remove(pReply);
    pReply->deleteLater();
}