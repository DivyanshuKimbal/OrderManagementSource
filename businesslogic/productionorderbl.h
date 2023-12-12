#ifndef PRODUCTIONORDERBL_H
#define PRODUCTIONORDERBL_H

#include <QByteArray>
#include <QSqlDatabase>

class ProductionOrderBL
{
public:
    ProductionOrderBL();
    ~ProductionOrderBL();

    QByteArray getOrderToView(const QMap<QString, QString>& queryMap);

    bool updateProductionStatus(const QString& moid, const QString& status);

private:
    QSqlDatabase dbInstance;
};

#endif // PRODUCTIONORDERBL_H
