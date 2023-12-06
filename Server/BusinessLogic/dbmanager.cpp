#include "dbmanager.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QTextStream>
#include <QDebug>

DBManager::DBManager(const QString &dbPath, QObject *parent)
    : QObject(parent), m_dbPath(dbPath)
{
}

DBManager::~DBManager()
{
    closeDatabase();
}

bool DBManager::openDatabase()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);

    if (!m_database.open()) {
        qDebug() << "Error opening database: " << m_database.databaseName();
        return false;
    }

    return true;
}

void DBManager::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DBManager::convertTableToCSV(const QString &tableName, const QString &csvFilePath)
{
    QFile csvFile(csvFilePath);
    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error opening CSV file:" << csvFile.errorString();
        return false;
    }

    QTextStream csvStream(&csvFile);

    QSqlQuery query("SELECT * FROM " + tableName, m_database);
    QSqlRecord record = query.record();

    // Write header row to CSV file
    int fieldCount = record.count();
    for (int i = 0; i < fieldCount; ++i) {
        csvStream << record.fieldName(i);
        if (i < fieldCount - 1) {
            csvStream << ",";
        }
    }
    csvStream << endl;

    // Write data rows to CSV file
    while (query.next()) {
        for (int i = 0; i < fieldCount; ++i) {
            csvStream << query.value(i).toString();
            if (i < fieldCount - 1) {
                csvStream << ",";
            }
        }
        csvStream << endl;
    }

    csvFile.close();
    return true;
}

void DBManager::convertAllTablesToCSV()
{
    if (!openDatabase()) {
        return;
    }

    QStringList tables = m_database.tables();
    closeDatabase();

    for (const QString &table : tables) {
        convertTableToCSV(table, table);
    }
}

bool DBManager::convertCustomTableToCSV(const QString &tableName, const QStringList &columns, const QString &csvFilePath)
{
    QFile csvFile(csvFilePath);
    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error opening CSV file:" << csvFile.errorString();
        return false;
    }

    QTextStream csvStream(&csvFile);

    // Write header row to CSV file
    int columnCount = columns.size();
    for (int i = 0; i < columnCount; ++i) {
        csvStream << columns.at(i);
        if (i < columnCount - 1) {
            csvStream << ",";
        }
    }
    csvStream << endl;

    QSqlQuery query("SELECT " + columns.join(",") + " FROM " + tableName, m_database);

    // Write data rows to CSV file
    while (query.next()) {
        for (int i = 0; i < columnCount; ++i) {
            csvStream << query.value(i).toString();
            if (i < columnCount - 1) {
                csvStream << ",";
            }
        }
        csvStream << endl;
    }

    csvFile.close();
    return true;
}

bool DBManager::convertQueryToCSV(const QString &queryStr, const QString &csvFilePath)
{
    QFile csvFile(csvFilePath);
    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error opening CSV file:" << csvFile.errorString();
        return false;
    }

    QTextStream csvStream(&csvFile);

    QSqlQuery query(queryStr, m_database);
    QSqlRecord record = query.record();

    // Write header row to CSV file
    int fieldCount = record.count();
    for (int i = 0; i < fieldCount; ++i) {
        csvStream << record.fieldName(i);
        if (i < fieldCount - 1) {
            csvStream << ",";
        }
    }
    csvStream << endl;

    // Write data rows to CSV file
    while (query.next()) {
        for (int i = 0; i < fieldCount; ++i) {
            csvStream << query.value(i).toString();
            if (i < fieldCount - 1) {
                csvStream << ",";
            }
        }
        csvStream << endl;
    }

    csvFile.close();
    return true;
}
