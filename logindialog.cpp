#include "logindialog.h"
#include "ui_logindialog.h"

#include <QWebView>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(urlChanged(QUrl)));
    connect(ui->webView, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
}

LoginDialog::~LoginDialog()
{
    QApplication::restoreOverrideCursor();
    delete ui;
}

void LoginDialog::loadStarted()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

void LoginDialog::loadFinished(bool /*b*/)
{
    QApplication::restoreOverrideCursor();
}

void LoginDialog::urlChanged(const QUrl &url)
{
    QString str = url.toString();
    if(str.indexOf("access_token") != -1)
    {
        QStringList query = str.split("#");
        QStringList lst = query[1].split("&");
        for (int i=0; i<lst.count(); i++ )
        {
            QStringList pair = lst[i].split("=");
            if (pair[0] == "access_token")
            {
                m_strAccessToken = pair[1];
                emit accessTokenObtained();
                QDialog::accept();
            }
        }
    }
    else if(str.indexOf("code=") != -1)
    {
        QStringList query = str.split("?");
        QStringList lst = query[1].split("&");
        for (int i=0; i<lst.count(); i++ )
        {
            QStringList pair = lst[i].split("=");
            if (pair[0] == "code")
            {
                m_strCode = pair[1];
                emit codeObtained();
                QDialog::accept();
            }
        }
    }
}

void LoginDialog::setLoginUrl(const QString& url)
{
   ui->webView->setUrl(QUrl(url));
}
