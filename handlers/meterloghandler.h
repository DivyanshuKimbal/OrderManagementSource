#ifndef METERLOGHANDLER_H
#define METERLOGHANDLER_H

#include "interface/irequesthandler.h"
#include "../businesslogic/meterlogbl.h"

class MeterLogHandler : public IRequestHandler
{
public:
    MeterLogHandler();

    // Implement virtual functions from IRequestHandler
    void handleRequest(QHttpRequest *request, QHttpResponse *response) override;

    void handleDeleteRequest(QHttpRequest *request, QHttpResponse *response) override;
    void handlePutRequest(QHttpRequest *request, QHttpResponse *response) override;
    void handlePostRequest(QHttpRequest *request, QHttpResponse *response) override;
    void handleGetRequest(QHttpRequest *request, QHttpResponse *response) override;

private:
    void sendJsonError(QHttpResponse *response, const QString &errorMessage);

    void processPhaseFailCount(QHttpRequest *request, QHttpResponse *response);
    void processPhaseFailData(QHttpRequest *request, QHttpResponse *response);
    void processDetailProductionLine(QHttpRequest *request, QHttpResponse *response);
    void processCountPassFail(QHttpRequest *request, QHttpResponse *response);

    MeterLogBL *m_meterLogBL;
};

#endif // METERLOGHANDLER_H
