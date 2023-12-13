#ifndef AUTHENTICATIONHANDLER_H
#define AUTHENTICATIONHANDLER_H

#include "qhttprequest.h"
#include "qhttpresponse.h"

#include <QByteArray>
#include <QSqlDatabase>

class AuthenticationHandler
{
public:
    AuthenticationHandler();
    ~AuthenticationHandler();

    void handleAuthRequest(QHttpRequest *request, QHttpResponse *response);

public:
    void sendJsonError(QHttpResponse *response, const QString &errorMessage);
    QByteArray generateSecureToken(const QString &username);

    QString m_username;
    QString m_password;

    QSqlDatabase dbInstance;
};

#endif // AUTHENTICATIONHANDLER_H
