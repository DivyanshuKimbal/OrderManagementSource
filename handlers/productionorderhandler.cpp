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

void ProductionOrderHandler::handlePostRequest(QHttpRequest *request, QHttpResponse *response)
{

}

void ProductionOrderHandler::handlePutRequest(QHttpRequest *request, QHttpResponse *response)
{
    QString path = request->url().path();
    request->storeBody();
    QObject::connect(request, &QHttpRequest::end, [=]() {
        if(path == QString(basePath) + "/updateProductionStatus")
        {
            processUpdateProductionStatus(request, response);
        }
        else
        {
            // Handle other endpoints
            response->writeHead(404);
            response->end(QByteArray("Endpoint Not Found"));
        }
    });
}

void ProductionOrderHandler::handleGetRequest(QHttpRequest *request, QHttpResponse *response)
{
    QString path = request->url().path();

    if(path == QString(basePath) + "/fetchProderOrder")
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

void ProductionOrderHandler::processOrderDetailView(QHttpRequest *request, QHttpResponse *response) {

    QString query = request->url().query();
    qDebug() << query;

    QUrlQuery urlQuery(request->url());
    QMap<QString, QString> queryMap;

    for(const auto& item : urlQuery.queryItems())
        queryMap.insert(item.first, item.second);

    QByteArray jsonData = m_productionOrderBL->getOrderToView(queryMap);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void ProductionOrderHandler::processUpdateProductionStatus(QHttpRequest *request, QHttpResponse *response) {
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

    if (!jsonObj.contains("id") || !jsonObj.contains("status")) {
        // Missing required fields
        sendJsonError(response, "Missing required fields in JSON.");
        return;
    }

    QString moid = jsonObj.value("moid").toString();
    QString productionStatus = jsonObj.value("productionStatus").toString();

    qDebug() << "id:" << moid;
    qDebug() << "status:" << productionStatus;

    bool retVal = m_productionOrderBL->updateProductionStatus(moid, productionStatus);

    if(retVal) {
        // Set response headers and send the JSON data as the response
        response->setHeader("Content-Type", "application/json");
        response->writeHead(QHttpResponse::STATUS_OK);
        response->end(QString("Update Done").toLatin1());
    }
    else
    {
        // Set response headers and send the JSON data as the response
        response->setHeader("Content-Type", "application/json");
        response->writeHead(QHttpResponse::STATUS_NOT_MODIFIED);
        response->end(QString("Update Failed!").toLatin1());
    }
}
