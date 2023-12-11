#include "productionorderbl.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

ProductionOrderBL::ProductionOrderBL()
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
ProductionOrderBL::~ProductionOrderBL() {
    // Close the database connection
    QSqlDatabase db = QSqlDatabase::database("OrderManagementConnection", false);
    if (db.isValid() && db.isOpen()) {
        db.close();
        qDebug() << "Database connection closed.";
    }
    QSqlDatabase::removeDatabase("OrderManagementConnection");
    qDebug() << "Removed database connection.";
}

QByteArray ProductionOrderBL::getDetailProductionLineOrder(const QString& ProductionStatus, const QString& ProductionId)
{
    QByteArray jsonData;

    QSqlQuery query(dbInstance);

    // Prepare the SELECT query to retrieve data based on ProductionID and ProductionStatus
    query.prepare("SELECT * FROM TABLE1 WHERE productionStatus = :status AND productionID = :id");
    query.bindValue(":status", ProductionStatus);
    query.bindValue(":id", ProductionId);

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error: Failed to fetch data - " << query.lastError().text();
        return jsonData;
    }

    QJsonArray jsonArray;

    // Iterate over the retrieved rows (there should be only one based on ProductionID and ProductionStatus)
    while (query.next()) {
        QJsonObject jsonObj;
        jsonObj["id"] = query.value(0).toInt();
        jsonObj["orderDate"] = query.value(1).toString();
        jsonObj["status"] = query.value(2).toString();
        jsonObj["productionStatus"] = query.value(3).toString();
        jsonObj["productionID"] = query.value(4).toString();
        jsonObj["manufacturingOrderNumber"] = query.value(5).toString();
        jsonObj["poNumber"] = query.value(6).toString();
        jsonObj["client"] = query.value(7).toString();
        jsonObj["sequenceFrom"] = query.value(8).toString();
        jsonObj["sequenceTo"] = query.value(9).toString();

        jsonArray.append(jsonObj);
    }

    // Create a JSON object and set the array
    QJsonObject jsonObject;
    jsonObject["data"] = jsonArray;

    // Convert JSON object to string
    QJsonDocument jsonDoc(jsonObject);
    jsonData = jsonDoc.toJson();

    return jsonData;
}

QByteArray ProductionOrderBL::getCountPassFail(const QString& DateFrom, const QString& DateTo) {
    // Convert QString date inputs to QDateTime
    QDateTime dateTimeFrom = QDateTime::fromString(DateFrom, "dd/MM/yyyy");
    QDateTime dateTimeTo = QDateTime::fromString(DateTo, "dd/MM/yyyy");

    // Prepare the SQL query to count 'Failed' and 'Passed' results within the specified date range
    QSqlQuery query(dbInstance);
    QString countQuery = QString("SELECT COUNT(CASE WHEN result='Failed' THEN 1 END) as failedCount, "
                                 "COUNT(CASE WHEN result='Passed' THEN 1 END) as passedCount "
                                 "FROM TABLE2 WHERE datetime BETWEEN :dateFrom AND :dateTo");
    query.prepare(countQuery);
    query.bindValue(":dateFrom", dateTimeFrom);
    query.bindValue(":dateTo", dateTimeTo);

    if (!query.exec()) {
        qDebug() << "Error executing query - " << query.lastError().text();
        return QByteArray(); // Return an empty JSON object
    }

    // Fetch the counts
    int failedCount = 0;
    int passedCount = 0;

    if (query.next()) {
        failedCount = query.value("failedCount").toInt();
        passedCount = query.value("passedCount").toInt();
    }

    // Create a JSON object with the counts
    QJsonObject jsonObject;
    jsonObject["Failed"] = failedCount;
    jsonObject["Passed"] = passedCount;

    // Create a JSON document and convert it to a QByteArray
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonData = jsonDocument.toJson();
    return jsonData;
}

QByteArray ProductionOrderBL::getFailedData(const QString& DateFrom, const QString& DateTo) {
    // Convert QString date inputs to QDateTime
    QDateTime dateTimeFrom = QDateTime::fromString(DateFrom, "dd/MM/yyyy");
    QDateTime dateTimeTo = QDateTime::fromString(DateTo, "dd/MM/yyyy");

    // Prepare the SQL query to retrieve all data for 'Failed' results within the specified date range
    QSqlQuery query(dbInstance);
    QString selectQuery = QString("SELECT * FROM TABLE2 WHERE result='Failed' AND datetime BETWEEN :dateFrom AND :dateTo");
    query.prepare(selectQuery);
    query.bindValue(":dateFrom", dateTimeFrom);
    query.bindValue(":dateTo", dateTimeTo);

    if (!query.exec()) {
        qDebug() << "Error executing query - " << query.lastError().text();
        return QByteArray(); // Return an empty JSON object
    }

    // Create a JSON array to store the result data
    QJsonArray jsonArray;

    // Fetch the data and append to the JSON array
    while (query.next()) {
        QJsonObject jsonObject;
        jsonObject["id"] = query.value("id").toInt();
        jsonObject["po"] = query.value("po").toString();
        jsonObject["mo"] = query.value("mo").toString();
        jsonObject["lastStage"] = query.value("lastStage").toString();
        jsonObject["serialNumber"] = query.value("serialNumber").toString();
        jsonObject["result"] = query.value("result").toString();
        jsonObject["datetime"] = query.value("datetime").toString();

        jsonArray.append(jsonObject);
    }

    QJsonObject jsonObject;
    jsonObject["data"] = jsonArray;
    // Create a JSON document with the array of failed data
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonData = jsonDocument.toJson();
    return jsonData;
}

QByteArray ProductionOrderBL::getCountByLastStageInDateRange(const QString& DateFrom, const QString& DateTo) {
    // Convert DateFrom and DateTo strings to QDateTime
    QDateTime fromDate = QDateTime::fromString(DateFrom, "dd/MM/yyyy");
    QDateTime toDate = QDateTime::fromString(DateTo, "dd/MM/yyyy").addDays(1).addSecs(-1); // Adjust to end of the day

    // Prepare the SQL query to get count of Passed and Failed results in the date range grouped by lastStage
    QSqlQuery query(dbInstance);
    QString selectQuery = QString("SELECT lastStage, "
                                  "SUM(CASE WHEN result = 'Passed' THEN 1 ELSE 0 END) AS PassedCount, "
                                  "SUM(CASE WHEN result = 'Failed' THEN 1 ELSE 0 END) AS FailedCount "
                                  "FROM TABLE2 "
                                  "WHERE datetime BETWEEN '%1' AND '%2' "
                                  "GROUP BY lastStage")
                            .arg(fromDate.toString(Qt::ISODate))
                            .arg(toDate.toString(Qt::ISODate));
    if (!query.exec(selectQuery)) {
        qDebug() << "Error executing query - " << query.lastError().text();
        return QByteArray(); // Return an empty JSON object
    }

    // Create a JSON array to store the result data
    QJsonArray jsonArray;

    // Fetch the data and append to the JSON array
    while (query.next()) {
        QJsonObject jsonObject;
        jsonObject["lastStage"] = query.value("lastStage").toString();
        jsonObject["PassedCount"] = query.value("PassedCount").toInt();
        jsonObject["FailedCount"] = query.value("FailedCount").toInt();

        jsonArray.append(jsonObject);
    }

    QJsonObject jsonObject;
    jsonObject["data"] = jsonArray;
    // Create a JSON document with the array of counts by lastStage within the date range
    QJsonDocument jsonDocument(jsonObject);
    return jsonDocument.toJson();
}

QByteArray ProductionOrderBL::getOrderToView(const QString& productionline) {
    QSqlQuery query(dbInstance);

    QString selectQuery = QString("SELECT "
                                  "moid, "
                                  "client, "
                                  "manufacturer, "
                                  "poNumber, "
                                  "loaNumber, "
                                  "loaNumber2, "
                                  "manufacturingOrderNumber, "
                                  "meterType, "
                                  "modelNumber, "
                                  "orderSize, "
                                  "orderDate, "
                                  "dispatchDate, "
                                  "productionLine, "
                                  "status, "
                                  "productionStatus, "
                                  "approval_Status, "
                                  "prefix, "
                                  "sequenceFrom, "
                                  "sequenceTo, "
                                  "comment, "
                                  "utility, "
                                  "meterSealCount, "
                                  "creatorUserId, "
                                  "canEdit, "
                                  "amisp, "
                                  "consigneeName, "
                                  "addressLine1, "
                                  "addressLine2, "
                                  "city, "
                                  "state, "
                                  "zipCode, "
                                  "contactNumber, "
                                  "meterFirmwareVersion, "
                                  "rfFirmwareVersion "
                                  "FROM ProductionOrder "
                                  "WHERE moid = '%1'")
                            .arg(productionline);



    if (!query.exec(selectQuery)) {
        qDebug() << "Error executing query - " << query.lastError().text();
        return QByteArray(); // Return an empty JSON object
    }

    // Retrieve column names from the query result
    QSqlRecord record = query.record();
    int columnCount = record.count();

    // Create a JSON array to store the result
    QJsonObject jsonObject;

    query.next();

    // Dynamically retrieve column names and values
    for (int i = 0; i < columnCount; ++i) {
        QString columnName = record.fieldName(i);
        QVariant columnValue = query.value(i);

        // Add the column name and value to the JSON object
        jsonObject[columnName] = QJsonValue::fromVariant(columnValue);
    }

    // Create a JSON document with the array of counts by lastStage within the date range
    QJsonDocument jsonDocument(jsonObject);
    return jsonDocument.toJson();
}

bool ProductionOrderBL::updateProductionStatus(const QString& moid, const QString& productionStatus) {
    QSqlQuery query(dbInstance);

    // Use a prepared statement with placeholders
    QString updateQuery = "UPDATE ProductionOrder SET productionStatus = :productionStatus WHERE moid = :moid";
    query.prepare(updateQuery);

    // Bind values to the placeholders
    query.bindValue(":productionStatus", productionStatus);
    query.bindValue(":moid", moid);

    // Execute the query
    if (query.exec()) {
        qDebug() << "Update successful!";
        return true;
    } else {
        qDebug() << "Error updating record:" << query.lastError().text();
        return false;
    }
}

