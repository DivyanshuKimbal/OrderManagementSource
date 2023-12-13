#include "authenticationhandler.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>
#include <QCryptographicHash>
#include <QRandomGenerator>

#define basePath "/auth/"

AuthenticationHandler::AuthenticationHandler()
{
    // Check if the database connection is already open
    if (QSqlDatabase::contains("OrderManagementConnection")) {
        qDebug() << "Database connection already open.";
        return;
    }

    // Add the SQLite database driver
    dbInstance = QSqlDatabase::addDatabase("QSQLITE", "OrderManagementConnection");
    dbInstance.setDatabaseName("OrderManagement.db");

    // Open the database
    if (!dbInstance.open()) {
        qDebug() << "Error: Unable to open the database";
        qDebug() << "Error details:" << dbInstance.lastError().text();
    } else {
        qDebug() << "Connected to the database.";
    }
}

// Destructor
AuthenticationHandler::~AuthenticationHandler() {
    // Close the database connection
    QSqlDatabase db = QSqlDatabase::database("OrderManagementConnection", false);
    if (db.isValid() && db.isOpen()) {
        db.close();
        qDebug() << "Database connection closed.";
    }
    QSqlDatabase::removeDatabase("OrderManagementConnection");
    qDebug() << "Removed database connection.";
}

void AuthenticationHandler::handleAuthRequest(QHttpRequest *request, QHttpResponse *response) {

    request->storeBody();
    QObject::connect(request, &QHttpRequest::end, [=]() {

        QJsonParseError error;
        QString bodyData = request->body();
        qDebug() << "Body Data :: " << bodyData;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(bodyData.toUtf8(), &error);
        qDebug() << "Entered AUTH FUNCTION";
        if (error.error != QJsonParseError::NoError) {
            // JSON parsing error
            qDebug() << "Failed 1";
            sendJsonError(response, "Error parsing JSON: " + error.errorString());
            return;
        }

        if (!jsonDoc.isObject()) {
            // Invalid JSON object
            qDebug() << "Failed 2";
            sendJsonError(response, "Invalid JSON object.");
            return;
        }

        // Extract DateFrom, DateTo, and api_key
        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("username") || !jsonObj.contains("password")) {
            // Missing required fields
            qDebug() << "Failed 3";
            sendJsonError(response, "Missing required fields in JSON.");
            return;
        }

        QString username = jsonObj.value("username").toString();
        QString password = jsonObj.value("password").toString();

        QSqlQuery query(dbInstance);

        if (!dbInstance.isOpen()) {
            // Reopen the database connection if it's not open
            if (!dbInstance.open()) {
                qDebug() << "Error: Unable to open the database";
                qDebug() << "Error details:" << dbInstance.lastError().text();
                sendJsonError(response, "Unable to open the database");
            }
        }

        QString queryString = QString("SELECT username, password FROM Credentials "
                               "WHERE username='%1' AND password='%2'").arg(username).arg(password);
        query.exec(queryString);

        if(!query.next()) {
            // Set response headers and send the JSON data as the response
            response->setHeader("Content-Type", "application/json");
            response->writeHead(QHttpResponse::STATUS_UNAUTHORIZED);
            response->end(QString("Authentication Failed! Invalid username or password").toLatin1());
        }
        else
        {
            // Generate a secure access token
            QByteArray accessToken = generateSecureToken(username);

            // Return the access token in JSON format
            QJsonObject responseJson;
            QString accessTokenStr = QString::fromUtf8(accessToken.toBase64());
            QString updateQuery = QString("UPDATE Credentials "
                                          "SET accessToken='%1' "
                                          "WHERE username='%2' AND "
                                          "password='%3'").arg(accessTokenStr).arg(username).arg(password);
            query.exec(updateQuery);

            responseJson["AccessToken"] = accessTokenStr;

            response->setHeader("Content-Type", "application/json");
            response->writeHead(QHttpResponse::STATUS_OK);
            response->end(QJsonDocument(responseJson).toJson());
        }
    });
}

QByteArray AuthenticationHandler::generateSecureToken(const QString &username)
{
    // Generate a random salt
    QByteArray salt;
    for (int i = 0; i < 32; ++i) {
        salt.append(QRandomGenerator::global()->bounded(256));
    }

    // Concatenate username, salt, and a secret key for added security
    QString dataToHash = username + salt + "This_secret_key_can_be_generated_using_any_key_clock_alternatives";

    // Use SHA-256 for hashing
    QByteArray hash = QCryptographicHash::hash(dataToHash.toUtf8(), QCryptographicHash::Sha256);

    // Combine salt and hash for the final token
    QByteArray token;
    token.append(salt);
    token.append(hash);

    return token.toBase64();
}

void AuthenticationHandler::sendJsonError(QHttpResponse *response, const QString &errorMessage) {
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
