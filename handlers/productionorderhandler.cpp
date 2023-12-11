#include "productionorderhandler.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"

#include <QDebug>
#include <QJsonParseError>
#include <QJsonObject>
#include <QUrlQuery>
#include <QSqlDatabase>
#include <QSqlError>

#define basePath "/productionOrder"

ProductionOrderHandler::ProductionOrderHandler()
{
    m_productionOrderBL = new ProductionOrderBL();
}

void ProductionOrderHandler::handleRequest(QHttpRequest *request, QHttpResponse *response) {
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

void ProductionOrderHandler::handleDeleteRequest(QHttpRequest *request, QHttpResponse *response)
{
    return;
}

void ProductionOrderHandler::handlePutRequest(QHttpRequest *request, QHttpResponse *response)
{
    return;
}

void ProductionOrderHandler::handlePostRequest(QHttpRequest *request, QHttpResponse *response)
{
    QString path = request->url().path();
    qDebug() << path;
    request->storeBody();
    QObject::connect(request, &QHttpRequest::end, [=]() {
        if(path == QString(basePath) + "/PassFailCount")
        {
            processCountPassFail(request, response);
        }
        else if(path == QString(basePath) + "/DetailProductionLine")
        {
            processDetailProductionLine(request, response);
        }
        else if(path == QString(basePath) + "/PhaseSerialFailList")
        {
            processPhaseFailData(request, response);
        }
        else if(path == QString(basePath) + "/PassFailRatio")
        {
            processPhaseFailCount(request, response);
        }
        else if(path == QString(basePath) + "/LastSerialNo")
        {

        }
    });
}

void ProductionOrderHandler::handleGetRequest(QHttpRequest *request, QHttpResponse *response)
{
    QString path = request->url().path();

    if(path == QString(basePath) + "/GetOrderToView")
    {
        processOrderDetailView(request, response);
    }
    else if (path == QString(basePath) + "/hello") {
        // Handle GET request for /hello endpoint
        QByteArray body = "Hello World! Divyanshu Kumar";
        response->setHeader("Content-Length", QString::number(body.size()));
        response->writeHead(200);
        response->end(body);
    } else if (path == QString(basePath) + "/info") {
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

void ProductionOrderHandler::processDetailProductionLine(QHttpRequest *request, QHttpResponse *response) {
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

    QByteArray jsonData = m_productionOrderBL->getDetailProductionLineOrder(ProductionStatus, ProductionId);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void ProductionOrderHandler::sendJsonError(QHttpResponse *response, const QString &errorMessage) {
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




void ProductionOrderHandler::processCountPassFail(QHttpRequest *request, QHttpResponse *response) {
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

    QByteArray jsonData = m_productionOrderBL->getCountPassFail(DateFrom, DateTo);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void ProductionOrderHandler::processPhaseFailData(QHttpRequest *request, QHttpResponse *response) {
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

    QByteArray jsonData = m_productionOrderBL->getFailedData(DateFrom, DateTo);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void ProductionOrderHandler::processPhaseFailCount(QHttpRequest *request, QHttpResponse *response) {
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

    QByteArray jsonData = m_productionOrderBL->getCountByLastStageInDateRange(DateFrom, DateTo);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void ProductionOrderHandler::processOrderDetailView(QHttpRequest *request, QHttpResponse *response) {

    QString query = request->url().query();
    qDebug() << query;

    QUrlQuery urlQuery(request->url());
    QString idValue = urlQuery.queryItemValue("Id");

    QByteArray jsonData = m_productionOrderBL->getOrderToView(idValue);


    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}
