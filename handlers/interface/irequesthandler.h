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
    // Add other pure virtual functions as needed for your handlers
};

#endif // IREQUESTHANDLER_H
