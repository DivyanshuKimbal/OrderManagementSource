#include "meterloghandler.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"

#include <QDebug>
#include <QJsonParseError>
#include <QJsonObject>
#include <QUrlQuery>
#include <QSqlDatabase>
#include <QSqlError>

#define basePath "/meterLog"

MeterLogHandler::MeterLogHandler()
{
    m_meterLogBL = new MeterLogBL();
}

void MeterLogHandler::handleRequest(QHttpRequest *request, QHttpResponse *response)
{

}

void MeterLogHandler::handleGetRequest(QHttpRequest *request, QHttpResponse *response)
{

}

void MeterLogHandler::handlePutRequest(QHttpRequest *request, QHttpResponse *response)
{

}

void MeterLogHandler::handlePostRequest(QHttpRequest *request, QHttpResponse *response)
{
    QString path = request->url().path();
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
        else
        {
            // Handle other endpoints
            response->writeHead(404);
            response->end(QByteArray("Endpoint Not Found"));
        }
    });
}

void MeterLogHandler::handleDeleteRequest(QHttpRequest *request, QHttpResponse *response)
{

}

void MeterLogHandler::sendJsonError(QHttpResponse *response, const QString &errorMessage) {
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

void MeterLogHandler::processDetailProductionLine(QHttpRequest *request, QHttpResponse *response) {
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

    QByteArray jsonData = m_meterLogBL->getDetailProductionLineOrder(ProductionStatus, ProductionId);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void MeterLogHandler::processCountPassFail(QHttpRequest *request, QHttpResponse *response) {
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

    QByteArray jsonData = m_meterLogBL->getCountPassFail(DateFrom, DateTo);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void MeterLogHandler::processPhaseFailData(QHttpRequest *request, QHttpResponse *response) {
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

    QByteArray jsonData = m_meterLogBL->getFailedData(DateFrom, DateTo);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}

void MeterLogHandler::processPhaseFailCount(QHttpRequest *request, QHttpResponse *response) {
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

    QByteArray jsonData = m_meterLogBL->getCountByLastStageInDateRange(DateFrom, DateTo);

    // Set response headers and send the JSON data as the response
    response->setHeader("Content-Type", "application/json");
    response->writeHead(QHttpResponse::STATUS_OK);
    response->end(jsonData);
}
