#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>

#include "qhttprequest.h"
#include "requesthandlerbl.h"

class RequestHandler : public QObject
{
    Q_OBJECT

public:
    RequestHandler(const QString& dbPath);
    void listenToServer();
    void handleDeleteRequest(QHttpRequest *request, QHttpResponse *response);
    void handlePutRequest(QHttpRequest *request, QHttpResponse *response);
    void handlePostRequest(QHttpRequest *request, QHttpResponse *response);
    void handleGetRequest(QHttpRequest *request, QHttpResponse *response);

private:
    RequestHandlerBL *m_requestHanlderBL;

    void sendJsonError(QHttpResponse *response, const QString &errorMessage);
    void processPhaseFailCount(QHttpRequest *request, QHttpResponse *response);
    void processPhaseFailData(QHttpRequest *request, QHttpResponse *response);
    void processDetailProductionLine(QHttpRequest *request, QHttpResponse *response);
    void processCountPassFail(QHttpRequest *request, QHttpResponse *response);

private slots:
    void handleRequest(QHttpRequest *request, QHttpResponse *response);
};

#endif // REQUESTHANDLER_H
