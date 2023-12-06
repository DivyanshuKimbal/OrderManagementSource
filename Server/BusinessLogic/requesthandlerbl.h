#ifndef REQUESTHANDLERBL_H
#define REQUESTHANDLERBL_H

#include "dbmanager.h"
#include "qhttprequest.h"

class RequestHandlerBL
{
public:
    RequestHandlerBL(const QString& dbPath);

    QByteArray getDetailProductionLineOrder(const QString& ProductionStatus, const QString& ProductionId);
    QByteArray getCountPassFail(const QString& DateFrom, const QString& DateTo);
    QByteArray getFailedData(const QString& DateFrom, const QString& DateTo);
    QByteArray getCountByLastStageInDateRange(const QString& DateFrom, const QString& DateTo) ;

private:
    DBManager *m_dbmanager;
};

#endif // REQUESTHANDLERBL_H
