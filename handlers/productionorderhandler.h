#ifndef PRODUCTIONORDERHANDLER_H
#define PRODUCTIONORDERHANDLER_H

#include "interface/irequesthandler.h"
#include "../businesslogic/productionorderbl.h"

class ProductionOrderHandler  : public IRequestHandler
{
public:
    ProductionOrderHandler();

    // Implement virtual functions from IRequestHandler
    void handleRequest(QHttpRequest *request, QHttpResponse *response) override;

private:
    void handleDeleteRequest(QHttpRequest *request, QHttpResponse *response);
    void handlePutRequest(QHttpRequest *request, QHttpResponse *response);
    void handlePostRequest(QHttpRequest *request, QHttpResponse *response);
    void handleGetRequest(QHttpRequest *request, QHttpResponse *response);

    void sendJsonError(QHttpResponse *response, const QString &errorMessage);
    void processPhaseFailCount(QHttpRequest *request, QHttpResponse *response);
    void processPhaseFailData(QHttpRequest *request, QHttpResponse *response);
    void processDetailProductionLine(QHttpRequest *request, QHttpResponse *response);
    void processCountPassFail(QHttpRequest *request, QHttpResponse *response);

    ProductionOrderBL *m_productionOrderBL;
};

#endif // PRODUCTIONORDERHANDLER_H
