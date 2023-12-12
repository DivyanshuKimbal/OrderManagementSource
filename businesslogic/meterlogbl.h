#ifndef METERLOGBL_H
#define METERLOGBL_H

#include <QByteArray>
#include <QSqlDatabase>

class MeterLogBL
{
public:
    MeterLogBL();
    ~MeterLogBL();

    QByteArray getDetailProductionLineOrder(const QString& ProductionStatus, const QString& ProductionId);
    QByteArray getCountPassFail(const QString& DateFrom, const QString& DateTo);
    QByteArray getFailedData(const QString& DateFrom, const QString& DateTo);
    QByteArray getCountByLastStageInDateRange(const QString& DateFrom, const QString& DateTo);

private:
    QSqlDatabase dbInstance;
};

#endif // METERLOGBL_H
