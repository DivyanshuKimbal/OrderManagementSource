#ifndef PRODUCTIONORDERBL_H
#define PRODUCTIONORDERBL_H

#include <QByteArray>
#include <QSqlDatabase>

class ProductionOrderBL
{
public:
    ProductionOrderBL();
    ~ProductionOrderBL();

    QByteArray getDetailProductionLineOrder(const QString& ProductionStatus, const QString& ProductionId);
    QByteArray getCountPassFail(const QString& DateFrom, const QString& DateTo);
    QByteArray getFailedData(const QString& DateFrom, const QString& DateTo);
    QByteArray getCountByLastStageInDateRange(const QString& DateFrom, const QString& DateTo);
    QByteArray getOrderToView(const QMap<QString, QString>& queryMap);

    bool updateProductionStatus(const QString& moid, const QString& status);

private:
    QSqlDatabase dbInstance;
};

#endif // PRODUCTIONORDERBL_H
