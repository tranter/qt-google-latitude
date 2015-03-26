# Introduction #

The qt-google-latitude example uses Google Latitude API. To use API you need to register your own application on Google. Do not worry: the procedure is very simple.


# Details #

You need to login to Google, so first you need to create simple Google account. Then you can visit the page

https://code.google.com/apis/console

there you can create your application. You need to check access to Latitude API.

Right now you need to register your application as a web server application.

**IMPORTANT!** For Redirect\_URI you should select any valid web server with any address on them.

Then  you can see credentials of your application. You need to copy and paste **Client\_ID** and **Redirect\_URI** and **Client\_secret** to the file **oauth2.cpp**.
```
OAuth2::OAuth2(QWidget* parent)
{
    m_strEndPoint = "https://accounts.google.com/o/oauth2/auth";
    m_strScope = "https://www.googleapis.com/auth/latitude.all.best"; //Access to Latitude service

    m_strClientID = "YOUR_CLIENT_ID_HERE";
    m_strClientSecret = "YOUR_CLIENT_SECRET_HERE";
    m_strRedirectURI = "YOUR_REDIRECT_URI_HERE";
    m_apiKeyGeocoding = "YOUR_API_KEY_HERE";

    m_pParent = parent;
    m_pLoginDialog = NULL;

    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    QSettings settings("ICS", "QtLatitude");
    m_strRefreshToken = settings.value("refresh_token", "").toString();
}
```

Additionally you can specify your company name in the file **form.cpp**.
```
Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    m_pOAuth2 = new OAuth2(this);

    m_strCompanyName = "YOU_COMPANY_NAME_HERE";
    m_strAppName = "QtLatitude";

    m_pManager = new LatitudeDataManager();
    m_pageLoaded = false;
    m_loginCompleted = false;

```

After that you can compile and run qt-google-latitude.