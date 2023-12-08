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

private:
    MeterLogBL *m_meterLogBL;
};

#endif // METERLOGHANDLER_H
