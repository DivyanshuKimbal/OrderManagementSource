#ifndef APICONTROLLER_H
#define APICONTROLLER_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include "qhttpserver.h"

#include "../handlers/interface/irequesthandler.h"

class APIController : public QObject
{
    Q_OBJECT

public:
    explicit APIController(QObject *parent = nullptr);

    // Method to start the server
    void startServer();

private slots:
    // Method to handle incoming requests
    void handleRequest(QHttpRequest *request, QHttpResponse *response);

private:
    // Method to register handlers for specific paths
    void registerHandlers();

    // Method to register a handler for a specific path
    void registerHandler(const QString &path, QSharedPointer<IRequestHandler> handler);

    // Map to store registered handlers based on paths
    QMap<QString, QSharedPointer<IRequestHandler>> handlerMap;

    // Server instance
    QHttpServer *server;
};

#endif // APICONTROLLER_H
