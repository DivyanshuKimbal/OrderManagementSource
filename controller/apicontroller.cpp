#include "apicontroller.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "../handlers/productionorderhandler.h"
#include "../handlers/meterloghandler.h"

APIController::APIController(QObject *parent) : QObject(parent), server(nullptr)
{

}

void APIController::registerHandlers()
{
    // Register handlers for specific paths within the APIController
    registerHandler("/productionOrder/hello", QSharedPointer<ProductionOrderHandler>::create());
    registerHandler("/meterLog", QSharedPointer<MeterLogHandler>::create());
    // Add more registrations as needed
}


void APIController::registerHandler(const QString &path, QSharedPointer<IRequestHandler> handler)
{
    // Register the handler for the specified path
    handlerMap[path] = handler;
}

void APIController::startServer()
{
    qDebug() << "Start Server point 1";
    // Register handlers upon creation
    registerHandlers();

    qDebug() << "Start Server point 2";

    // Initialize the server in the constructor
    server = new QHttpServer(this);

    qDebug() << "Start Server point 3";

    // Connect the server's newRequest signal to the handleRequest slot
    connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handleRequest(QHttpRequest*, QHttpResponse*)));

    qDebug() << "Start Server point 4";

    // Start the server on port 8080
    server->listen(QHostAddress::Any, 8080);
    qDebug() << "Server started on port 8080";
}

void APIController::handleRequest(QHttpRequest *request, QHttpResponse *response)
{
    // Extract path from the incoming request URL
    QString path = request->url().path();

    qDebug() << path;

    // Find the handler for the specified path
    auto handler = handlerMap.value(path);

    if (handler.isNull()) {
        // No handler found for the path, return a 404 response
        response->writeHead(404);
        response->end("Not Found");
    } else {
        // Delegate handling to the registered handler
        handler->handleRequest(request, response);
    }
}
