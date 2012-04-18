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
    void setCompanyName(const QString& companyName) {m_strCompanyName = companyName;}
    void setAppName(const QString& appName) {m_strAppName = appName;}

    QString geocodingKey();

    QString loginUrl();
    QString permanentLoginUrl();

    QString getAccessToken() {return m_strAccessToken;}
    void setAccessToken(const QString& access_token) {m_strAccessToken = access_token;}
    QString getRefreshToken() { return m_strRefreshToken; }
    void setRefreshToken(const QString& refresh_token) {m_strRefreshToken = refresh_token;}

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

    QString m_strCompanyName;
    QString m_strAppName;

    LoginDialog* m_pLoginDialog;
    QWidget* m_pParent;

    QNetworkAccessManager * m_pNetworkAccessManager;

    QString m_apiKeyGeocoding;

};

#endif // OAUTH2_H
