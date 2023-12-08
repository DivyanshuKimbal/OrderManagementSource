#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>

class DBManager : public QObject
{
    Q_OBJECT

public:
    explicit DBManager(const QString &dbPath, QObject *parent = nullptr);
    ~DBManager();

    bool openDatabase();
    void closeDatabase();

    bool convertTableToCSV(const QString &tableName, const QString &csvFileName);
    void convertAllTablesToCSV();
    bool convertCustomTableToCSV(const QString &tableName, const QStringList &columns, const QString &csvFileName);
    bool convertQueryToCSV(const QString &query, const QString &csvFileName);

private:
    QString m_dbPath;
    QSqlDatabase m_database;
};

#endif // DBMANAGER_H
