#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QUrl>
#include <QVariant>
#include "oauth2.h"

namespace Ui {
    class Form;
}

class LatitudeDataManager;
class QSettings;

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

    void startLogin(bool bForce); //Show login dialog even if there is gotten already access_token.
    void showCurrentLocation();
    void getHistoryLocation();

private slots:
    void loginDone();
    void onErrorOccured(const QString& error);
    void onLoadFinished(bool ok);
    void onCurrentLocationReady();
    void onLocationHistoryReady();
    void onAddressLocationsListReady();
    void onHistoryCellDoubleClicked(int row,int column);
    void onClickedGo();
    void onClickedInsert();
    void onDelete();
    void onInsertCurrentLocation();
    void twHistoryCustomMenu(const QPoint& point);
    void onLocationInserted();
    void onLocationDeleted();

private:
    Ui::Form *ui;
    LatitudeDataManager* m_pManager;
    OAuth2* m_pOauth2;
    QSettings* m_pSettings;
    bool m_pageLoaded;
    bool m_loginCompleted;
    QVariant m_location;
    QAction* m_aDelete;
    QAction* m_aInsertCurrentLocation;

    QString convertMs2String(qlonglong ms);
    void gotoLocation(const QVariant& location);
    void saveSettings();
};

#endif // FORM_H
