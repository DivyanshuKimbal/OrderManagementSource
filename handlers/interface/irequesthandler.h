// include/handlers/interface/irequesthandler.h
#ifndef IREQUESTHANDLER_H
#define IREQUESTHANDLER_H

#include <QObject>

class QHttpRequest;
class QHttpResponse;

class IRequestHandler : public QObject
{
    Q_OBJECT

public:
    virtual void handleRequest(QHttpRequest *request, QHttpResponse *response) = 0;

    virtual void handleDeleteRequest(QHttpRequest *request, QHttpResponse *response) = 0;
    virtual void handlePutRequest(QHttpRequest *request, QHttpResponse *response) = 0;
    virtual void handlePostRequest(QHttpRequest *request, QHttpResponse *response) = 0;
    virtual void handleGetRequest(QHttpRequest *request, QHttpResponse *response) = 0;
};

#endif // IREQUESTHANDLER_H
