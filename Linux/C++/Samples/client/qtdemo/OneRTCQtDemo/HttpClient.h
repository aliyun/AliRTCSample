#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <memory>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class HttpClient : public QObject
{
    Q_OBJECT
public:

    explicit HttpClient(QObject *parent = nullptr);

    void get(const QString &urlString,
             const QMap<QString, QString> &headers,
             const QMap<QString, QString> &params);

    void post(const QString &urlString,
             const QMap<QString, QString> &headers,
             const QByteArray &data);

    void head(const QString &urlString,
             const QMap<QString, QString> &headers,
             const QMap<QString, QString> &params);

signals:
    void onResponse(QString response);
    void onError(int code, QString desc);

private slots:
    void handleFinished(QNetworkReply* reply);
    // void onError(QNetworkReply::NetworkError err);

private:
    void addQueryParams(QUrl &url, const QMap<QString, QString> &params);
    void addHeaders(QNetworkRequest &request, const QMap<QString, QString> &headers);

private:
    std::unique_ptr<QNetworkAccessManager> manager_;
};

#endif // HTTPCLIENT_H
