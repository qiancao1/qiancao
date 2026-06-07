#ifndef WINHTTPREQUEST_H
#define WINHTTPREQUEST_H

#include <QString>
#include <QByteArray>
#include <QMap>

class WinHttpRequest
{
public:
    enum Method { Get, Post, Head, Put, Delete, Options, Trace, Connect };

    WinHttpRequest();
    ~WinHttpRequest();

    // 链式设置接口
    WinHttpRequest& setUrl(const QString& url);
    WinHttpRequest& setMethod(Method method);
    WinHttpRequest& setBody(const QByteArray& body);
    WinHttpRequest& setBody(const QString& text);
    WinHttpRequest& addHeader(const QString& name, const QString& value);
    WinHttpRequest& setContentType(const QString& contentType);
    WinHttpRequest& setTimeout(int msecs);          // 总超时（毫秒）
    WinHttpRequest& setCookie(const QString& cookie);
    WinHttpRequest& setVerifyCertificate(bool verify); // 是否验证证书
    bool parseUrlRaw(const QString& url, QString& host, int& port, QString& path, bool& isHttps);
    // 执行请求（同步阻塞）
    bool exec();

    // 获取结果
    QByteArray body() const;
    int statusCode() const;
    QString errorString() const;
    QString responseHeaders() const;

private:
    bool sendRequest(const QString& url);
    bool parseUrl(const QString& url, QString& host, int& port, QString& path, bool& isHttps) const;
    QString buildHeadersString() const;

    // 参数
    QString m_url;
    Method m_method = Get;
    QByteArray m_body;
    QMap<QString, QString> m_headers;
    int m_timeoutMs = 30000;
    QString m_cookie;
    bool m_verifyCert = true;

    // 结果
    QByteArray m_resultBody;
    int m_statusCode = 0;
    QString m_errorString;
    QString m_responseHeaders;

    // WinHTTP 句柄
    void* m_hSession = nullptr;
    void* m_hConnect = nullptr;
    void* m_hRequest = nullptr;
};

#endif // WINHTTPREQUEST_H