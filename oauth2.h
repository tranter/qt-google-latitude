#ifndef OAUTH2_H
#define OAUTH2_H

#include <QString>
#include <QObject>

class LoginDialog;
class QNetworkAccessManager;
class QNetworkReply;
class QSettings;

class OAuth2 : public QObject
{
    Q_OBJECT

public:
    OAuth2(QWidget* parent = 0);
    bool isAuthorized();
    void startLogin(bool bForce);

    QString geocodingKey();

    QString loginUrl();
    QString permanentLoginUrl();

    QString accessToken() {return m_strAccessToken;}
    void setAccessToken(const QString& access_token) {m_strAccessToken = access_token;}
    QString refreshToken() { return m_strRefreshToken; }
    void setRefreshToken(const QString& refresh_token) {m_strRefreshToken = refresh_token;}

    void setSettings(QSettings* p) {m_pSettings = p;}

signals:
    void loginDone();
    void sigErrorOccured(const QString&);

private slots:
    void accessTokenObtained();
    void codeObtained();
    void replyFinished(QNetworkReply*);
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

    LoginDialog* m_pLoginDialog;
    QWidget* m_pParent;

    QSettings* m_pSettings;
    QNetworkAccessManager * m_pNetworkAccessManager;

    QString m_apiKeyGeocoding;

};

#endif // OAUTH2_H
