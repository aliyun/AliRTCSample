#include "HttpClient.h"
#include <QUrlQuery>
#include <QThread>
#include <QElapsedTimer>
#include "Utils.h"

static QElapsedTimer g_timer_;

HttpClient::HttpClient(QObject *parent)
    : QObject{parent}
{
    manager_ = std::make_unique<QNetworkAccessManager>(this);
    connect(manager_.get(), SIGNAL(finished(QNetworkReply*)), this, SLOT(handleFinished(QNetworkReply*)));
}

void HttpClient::get(const QString &urlString,
                     const QMap<QString, QString> &headers,
                     const QMap<QString, QString> &params) {
    qDebug("[%d] HttpClient::get, urlString: %s\n", CurrentThreadId(), urlString.toStdString().c_str());
    g_timer_.start();

    QUrl url(urlString);
    addQueryParams(url, params);
    QNetworkRequest request(url);
    addHeaders(request, headers);

    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    manager_->get(request);

    qDebug() << "[" << CurrentThreadId() << "] HttpClient::get, costms: " << g_timer_.elapsed();
}

void HttpClient::post(const QString &urlString,
                      const QMap<QString, QString> &headers,
                      const QByteArray &data) {
    QUrl url(urlString);
    QNetworkRequest request(url);
    addHeaders(request, headers);
    manager_->post(request, data);
}

void HttpClient::head(const QString &urlString,
          const QMap<QString, QString> &headers,
                      const QMap<QString, QString> &params) {
    QUrl url(urlString);
    addQueryParams(url, params);
    QNetworkRequest request(url);
    addHeaders(request, headers);
    manager_->head(request);
}

void HttpClient::handleFinished(QNetworkReply *reply)
{
    qDebug() << "[" << CurrentThreadId() << "] HttpClient::get, whole request costms: " << g_timer_.elapsed();

    QString response = reply->readAll();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug("[%d] HttpClient::handleFinished statusCode: %d, reply:%s\n", CurrentThreadId(), statusCode, qPrintable(response));
    if (reply->error() == QNetworkReply::NoError && statusCode == 200) {
        emit onResponse(response);
    } else {
        QString desc = reply->errorString();
        if (desc.isEmpty()) desc = reply->readAll();
        emit onError(statusCode, desc);
    }

    reply->deleteLater();
}

void HttpClient::addQueryParams(QUrl &url, const QMap<QString, QString> &params)
{
    QUrlQuery query;
    QMapIterator<QString, QString> iterator(params);
    while (iterator.hasNext()) {
        iterator.next();
        query.addQueryItem(iterator.key(), iterator.value());
    }
    url.setQuery(query);
}

void HttpClient::addHeaders(QNetworkRequest &request, const QMap<QString, QString> &headers)
{
    QMapIterator<QString, QString> headerIter(headers);
    while (headerIter.hasNext()) {
        headerIter.next();
        request.setRawHeader(QByteArray::fromStdString(headerIter.key().toStdString()),
                             QByteArray::fromStdString(headerIter.value().toStdString()));
    }
}

