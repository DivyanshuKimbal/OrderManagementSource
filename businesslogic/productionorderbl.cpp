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

QByteArray ProductionOrderBL::getOrderToView(const QMap<QString, QString>& queryMap) {
    QSqlQuery query(dbInstance);

    QString selectQuery = QString("SELECT * FROM ProductionOrder ");

    if(!queryMap.empty()) {
        QString whereClauseLogic = "WHERE ";
        QStringList columnNames = queryMap.keys();
        for(int i = 0; i < columnNames.size(); i++) {
            QString columnName = columnNames[i];
            QString columnVal = queryMap.value(columnName);
            whereClauseLogic += columnName + "=" + columnVal;
            if(i < columnNames.size() - 1)
                whereClauseLogic += " AND ";
        }
        selectQuery += whereClauseLogic;
    }

    qDebug() << "QUERY = " << selectQuery;

    if (!query.exec(selectQuery)) {
        qDebug() << "Error executing query - " << query.lastError().text();
        return QByteArray(); // Return an empty JSON object
    }

    // Retrieve column names from the query result
    QSqlRecord record = query.record();
    int columnCount = record.count();

    // Create a JSON array to store the result
    QJsonArray jsonArray;

    while(query.next()) {
        // Dynamically retrieve column names and values
        QJsonObject jsonObject;
        for (int i = 0; i < columnCount; ++i) {
            QString columnName = record.fieldName(i);
            QVariant columnValue = query.value(i);

            // Add the column name and value to the JSON object
            jsonObject[columnName] = QJsonValue::fromVariant(columnValue);
        }
        jsonArray.append(jsonObject);
    }

    QJsonObject jsonObject;
    jsonObject["record"] = jsonArray;

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

