#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include "requesthandler.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"

RequestHandler::RequestHandler(const QString& dbPath)
{
    m_requestHanlderBL = new RequestHandlerBL(dbPath);
}

void RequestHandler::listenToServer() {
    QHttpServer *server = new QHttpServer(this);
    connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handleRequest(QHttpRequest*, QHttpResponse*)));

    server->listen(QHostAddress::Any, 8080);
    qDebug() << "Server started on port" ;
}

void RequestHandler::handleRequest(QHttpRequest *request, QHttpResponse *response) {
    switch (request->method()) {
        case QHttpRequest::HTTP_DELETE:
            handleDeleteRequest(request, response);
            break;

        case QHttpRequest::HTTP_GET:
            handleGetRequest(request, response);
            break;

        case QHttpRequest::HTTP_POST:
            handlePostRequest(request, response);
            break;

        case QHttpRequest::HTTP_PUT:
            handlePutRequest(request, response);
            break;

        default:
            response->writeHead(400);
            response->end("Unsupported HTTP method");
            break;
    }
}

void RequestHandler::handleDeleteRequest(QHttpRequest *request, QHttpResponse *response)
{
    return;
}

void RequestHandler::handlePutRequest(QHttpRequest *request, QHttpResponse *response)
{
    return;
}

void RequestHandler::handlePostRequest(QHttpRequest *request, QHttpResponse *response)
{
    qDebug()<<request->url().path();
    request->storeBody();
    QObject::connect(request, &QHttpRequest::end, [=]() {
        if(request->url().path()=="/api/PassFailCount")
        {
            processCountPassFail(request, response);
        }
        else if(request->url().path()=="/api/DetailProductionLine")
        {
            processDetailProductionLine(request, response);
        }
        else if(request->url().path()=="/api/PhaseSerialFailList")
        {
            processPhaseFailData(request, response);
        }
        else if(request->url().path()=="/api/PassFailRatio")
        {
            processPhaseFailCount(request, response);
        }
        else if(request->url().path()=="/api/LastSerialNo")
        {

        }
    });
}

void RequestHandler::handleGetRequest(QHttpRequest *request, QHttpResponse *response)
{
    if (request->url().path() == "/hello") {
        // Handle GET request for /hello endpoint
        QByteArray body = "Hello World! Divyanshu Kumar";
        response->setHeader("Content-Length", QString::number(body.size()));
        response->writeHead(200);
        response->end(body);
    } else if (request->url().path() == "/info") {
        // Handle GET request for /info endpoint
        QByteArray body = "Information Endpoint";
        response->setHeader("Content-Length", QString::number(body.size()));
        response->writeHead(200);
        response->end(body);
    } else {
        // Handle other endpoints
        response->writeHead(404);
        response->end(QByteArray("Endpoint Not Found"));
    }
}

void RequestHandler::processDetailProductionLine(QHttpRequest *request, QHttpResponse *response) {
    QJsonParseError error;
    QString bodyData = request->body();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(bodyData.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        // JSON parsing error
        sendJsonError(response, "Error parsing JSON: " + error.errorString());
        return;
    }

    if (!jsonDoc.isObject()) {
        // Invalid JSON object
        sendJsonError(response, "Invalid JSON object.");
        return;
    }

    // Extract productId and productStatus
    QJsonObject jsonObj = jsonDoc.object();

    if (!jsonObj.contains("ProductionId") || !jsonObj.contains("ProductionStatus") || !jsonObj.contains("api_key")) {
        // Missing required fields
        sendJsonError(response, "Missing required fields in JSON.");
        return;
    }

    QString ProductionStatus = jsonObj.value("ProductionStatus").toString();
    QString ProductionId = jsonObj.value("ProductionId").toString();
    QString api_key = jsonObj.value("api_key").toString();

    if (api_key != "5") {
        // API Key mismatch
        sendJsonError(response, "API Key mismatch.");
        return;
    }

    qDebug() << "ProductionStatus:" << ProductionStatus;
    qDebug() << "ProductionId:" << ProductionId;

    QByteArray jsonData = m_requestHanlderBL->getDetailProductionLineOrder(ProductionStatus, ProductionId);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void RequestHandler::sendJsonError(QHttpResponse *response, const QString &errorMessage) {
    QJsonObject errorObj;
    errorObj["error"] = "Bad Request";
    errorObj["message"] = errorMessage;

    QJsonObject jsonObject;
    jsonObject["error"] = errorObj;

    // Convert JSON object to string
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson();

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_BAD_REQUEST);
    response->end(jsonData);
}




void RequestHandler::processCountPassFail(QHttpRequest *request, QHttpResponse *response) {
    QJsonParseError error;
    QString bodyData = request->body();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(bodyData.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        // JSON parsing error
        sendJsonError(response, "Error parsing JSON: " + error.errorString());
        return;
    }

    if (!jsonDoc.isObject()) {
        // Invalid JSON object
        sendJsonError(response, "Invalid JSON object.");
        return;
    }

    // Extract DateFrom, DateTo, and api_key
    QJsonObject jsonObj = jsonDoc.object();

    if (!jsonObj.contains("DateFrom") || !jsonObj.contains("DateTo") || !jsonObj.contains("api_key")) {
        // Missing required fields
        sendJsonError(response, "Missing required fields in JSON.");
        return;
    }

    QString DateFrom = jsonObj.value("DateFrom").toString();
    QString DateTo = jsonObj.value("DateTo").toString();
    QString api_key = jsonObj.value("api_key").toString();

    if (api_key != "1") {
        // API Key mismatch
        sendJsonError(response, "API Key mismatch.");
        return;
    }

    qDebug() << "DateFrom:" << DateFrom;
    qDebug() << "DateTo:" << DateTo;

    QByteArray jsonData = m_requestHanlderBL->getCountPassFail(DateFrom, DateTo);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void RequestHandler::processPhaseFailData(QHttpRequest *request, QHttpResponse *response) {
    QJsonParseError error;
    QString bodyData = request->body();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(bodyData.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        // JSON parsing error
        sendJsonError(response, "Error parsing JSON: " + error.errorString());
        return;
    }

    if (!jsonDoc.isObject()) {
        // Invalid JSON object
        sendJsonError(response, "Invalid JSON object.");
        return;
    }

    // Extract DateFrom, DateTo, and api_key
    QJsonObject jsonObj = jsonDoc.object();

    if (!jsonObj.contains("DateFrom") || !jsonObj.contains("DateTo") || !jsonObj.contains("api_key")) {
        // Missing required fields
        sendJsonError(response, "Missing required fields in JSON.");
        return;
    }

    QString DateFrom = jsonObj.value("DateFrom").toString();
    QString DateTo = jsonObj.value("DateTo").toString();
    QString api_key = jsonObj.value("api_key").toString();

    if (api_key != "2") {
        // API Key mismatch
        sendJsonError(response, "API Key mismatch.");
        return;
    }

    qDebug() << "DateFrom:" << DateFrom;
    qDebug() << "DateTo:" << DateTo;

    QByteArray jsonData = m_requestHanlderBL->getFailedData(DateFrom, DateTo);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void RequestHandler::processPhaseFailCount(QHttpRequest *request, QHttpResponse *response) {
    QJsonParseError error;
    QString bodyData = request->body();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(bodyData.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        // JSON parsing error
        sendJsonError(response, "Error parsing JSON: " + error.errorString());
        return;
    }

    if (!jsonDoc.isObject()) {
        // Invalid JSON object
        sendJsonError(response, "Invalid JSON object.");
        return;
    }

    // Extract DateFrom, DateTo, and api_key
    QJsonObject jsonObj = jsonDoc.object();

    if (!jsonObj.contains("DateFrom") || !jsonObj.contains("DateTo") || !jsonObj.contains("api_key")) {
        // Missing required fields
        sendJsonError(response, "Missing required fields in JSON.");
        return;
    }

    QString DateFrom = jsonObj.value("DateFrom").toString();
    QString DateTo = jsonObj.value("DateTo").toString();
    QString api_key = jsonObj.value("api_key").toString();

    if (api_key != "3") {
        // API Key mismatch
        sendJsonError(response, "API Key mismatch.");
        return;
    }

    qDebug() << "DateFrom:" << DateFrom;
    qDebug() << "DateTo:" << DateTo;

    QByteArray jsonData = m_requestHanlderBL->getCountByLastStageInDateRange(DateFrom, DateTo);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}
