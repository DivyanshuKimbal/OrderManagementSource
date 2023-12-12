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
    registerHandler("/productionOrder/", QSharedPointer<ProductionOrderHandler>::create());
    registerHandler("/meterLog/", QSharedPointer<MeterLogHandler>::create());
    // Add more registrations as needed
}


void APIController::registerHandler(const QString &path, QSharedPointer<IRequestHandler> handler)
{
    // Register the handler for the specified path
    handlerMap[path] = handler;
}

void APIController::startServer()
{
    // Register handlers upon creation
    registerHandlers();

    // Initialize the server in the constructor
    server = new QHttpServer(this);

    // Connect the server's newRequest signal to the handleRequest slot
    connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handleRequest(QHttpRequest*, QHttpResponse*)));

    // Start the server on port 8080
    server->listen(QHostAddress::Any, 8080);
    qDebug() << "Server started on port 8080";
}

void APIController::handleRequest(QHttpRequest *request, QHttpResponse *response)
{
    // Extract path from the incoming request URL
    QString path = request->url().path();

    // Find the handler for the specified path
    QSharedPointer<IRequestHandler> handler = nullptr;

    for (auto it = handlerMap.begin(); it != handlerMap.end(); ++it) {
        if (path.startsWith(it.key())) {
            handler = it.value();
            break;
        }
    }

    if (handler.isNull()) {
        // No handler found for the path, return a 404 response
        response->writeHead(404);
        response->end("Not Found");
    } else {
        // Delegate handling to the registered handler
        handler->handleRequest(request, response);
    }
}
