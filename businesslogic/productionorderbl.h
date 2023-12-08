#ifndef PRODUCTIONORDERBL_H
#define PRODUCTIONORDERBL_H

#include <QByteArray>

class ProductionOrderBL
{
public:
    ProductionOrderBL();

    QByteArray getDetailProductionLineOrder(const QString& ProductionStatus, const QString& ProductionId);
    QByteArray getCountPassFail(const QString& DateFrom, const QString& DateTo);
    QByteArray getFailedData(const QString& DateFrom, const QString& DateTo);
    QByteArray getCountByLastStageInDateRange(const QString& DateFrom, const QString& DateTo) ;
};

#endif // PRODUCTIONORDERBL_H
