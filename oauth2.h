#ifndef OAUTH2_H
#define OAUTH2_H

#include <QString>
#include <QObject>

class LoginDialog;
class QNetworkAccessManager;
class QNetworkReply;

class OAuth2 : public QObject
{
    Q_OBJECT

public:
    OAuth2(QWidget* parent = 0);
    QString accessToken();
    bool isAuthorized();
    void startLogin(bool bForce);

    QString geocodingKey();

    QString loginUrl();
    QString permanentLoginUrl();

signals:
    void loginDone();
    void errorOccured(const QString&);

private slots:
    void accessTokenObtained();
    void codeObtained();
    void replyFinished(QNetworkReply*);

private:
    void getAccessTokenFromRefreshToken();


private:
    QString m_strAccessToken;
    QString m_strRefreshToken;
    QString m_strCode;

    QString m_strEndPoint;
    QString m_strScope;
    QString m_strClientID;
    QString m_strClientSecret;
    QString m_strRedirectURI;
    QString m_strResponseType;

    LoginDialog* m_pLoginDialog;
    QWidget* m_pParent;

    QNetworkAccessManager * m_pNetworkAccessManager;

    QString m_apiKeyGeocoding;

};

#endif // OAUTH2_H
