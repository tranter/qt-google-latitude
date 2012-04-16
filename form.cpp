#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QSettings>
#include <QAction>
#include <QMenu>

#include "form.h"
#include "ui_form.h"
#include "latitude_data_manager.h"
#include "selectaddressdlg.h"

/*! Workaround pan/zoom problem with Google Map and QWebView:
 *  http://stackoverflow.com/questions/6184240/map-doesnt-respond-on-mouse-clicks-with-google-maps-api-v3-and-qwebview
 */
class MyWebPage : public QWebPage
{
    virtual QString userAgentForUrl(const QUrl&) const {
        return "Chrome/1.0";
    }
};
/*! Process authentication and first page loading are both asynchronouse. But for sequential event processing we should
 *  show current location after authorization and loading first page. But how to determine event when page is fully loaded?
 *  It is a problem because page can contain different frames, different AJAX scripts etc, so signal loadFinished will be
 *  present many times. So we made content of first page maximally simplified: remove all elements except main Google script
 *  and main div declaration. As a result we get only 1 signal for this page. After receiving loadFinished signal we
 *  disconnect it to prevent further handling.
 *
 *  For serialization use simple 2 flags and loading current location begin either after login completion or
 *  first page loading.
 */
Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    m_pManager = new LatitudeDataManager();
    m_pageLoaded = false;
    m_loginCompleted = false;

    ui->twHistory->setRowCount(0);
    ui->twHistory->setColumnCount(3);
    ui->twHistory->setColumnWidth(0,110);
    ui->twHistory->setColumnWidth(1,50);
    ui->twHistory->setColumnWidth(2,50);

    m_pOauth2 = new OAuth2(this);

    connect(m_pOauth2, SIGNAL(loginDone()), this, SLOT(loginDone()));
    connect(m_pManager, SIGNAL(errorOccured(QString)),this,SLOT(onErrorOccured(QString)));
    connect(m_pManager, SIGNAL(currentLocationReady()),this,SLOT(onCurrentLocationReady()));
    connect(m_pManager, SIGNAL(locationHistoryReady()),this,SLOT(onLocationHistoryReady()));
    connect(m_pManager, SIGNAL(addressLocationsListReady()),this,SLOT(onAddressLocationsListReady()));
    connect(m_pManager, SIGNAL(locationInserted()),this,SLOT(onLocationInserted()));
    connect(m_pManager, SIGNAL(locationDeleted()),this,SLOT(onLocationDeleted()));
    connect(ui->webView, SIGNAL(loadFinished(bool)),this, SLOT(onLoadFinished(bool)));
    connect(ui->twHistory, SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onHistoryCellDoubleClicked(int,int)));
    connect(ui->pbGo, SIGNAL(clicked()),this, SLOT(onClickedGo()));
    connect(ui->pbInsert, SIGNAL(clicked()),this, SLOT(onClickedInsert()));

    // Load settings
    m_pSettings = new QSettings("ICS", "Google API Latitude Client");
    QVariant zoom = m_pSettings->value("zoom");
    int pos = ui->cbZoom->findText(zoom.toString());
    if (pos == -1) {
        pos = 0;
    }
    ui->cbZoom->setCurrentIndex(pos);
    QVariant map_type = m_pSettings->value("map_type");
    pos = ui->cbMapType->findText(map_type.toString());
    if (pos == -1) {
        pos = 0;
    }
    ui->cbMapType->setCurrentIndex(pos);
    QVariant addresses = m_pSettings->value("list_addresses");
    ui->cbAddress->addItems(addresses.toStringList());
    QVariant address = m_pSettings->value("address");
    pos = ui->cbAddress->findText(address.toString());
    if (pos == -1) {
        pos = 0;
    }
    ui->cbAddress->setCurrentIndex(pos);

    ui->webView->setPage(new MyWebPage());
    ui->webView->setUrl(QUrl("qrc:/html/google_maps.html"));

    m_aDelete = new QAction("Delete Location", ui->twHistory);
    connect(m_aDelete,SIGNAL(triggered()),this,SLOT(onDelete()));
    m_aInsertCurrentLocation = new QAction("Insert Location as Current", ui->twHistory);
    connect(m_aInsertCurrentLocation,SIGNAL(triggered()),this,SLOT(onInsertCurrentLocation()));

    ui->twHistory->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->twHistory,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(twHistoryCustomMenu(QPoint)));
}

Form::~Form()
{
    saveSettings();
    delete m_pSettings;
    delete m_pManager;
    delete ui;
}

void Form::startLogin(bool bForce)
{
    m_pOauth2->startLogin(bForce);
}

void Form::loginDone()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    m_loginCompleted = true;
    if (m_pageLoaded) {
        qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "begin get current location...";
        m_pManager->getCurrentLocation(m_pOauth2->accessToken());
    }
}
/*! \brief Handler for signal errorOccured
 *
 */
void Form::onErrorOccured(const QString& error)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << error;
    if(error.indexOf("Invalid Credentials") != -1)
    {
        startLogin(true);
    }
    else
    {
        QMessageBox::warning(this, tr("Error occured"), error);
    }
}

void Form::showCurrentLocation()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    m_pManager->getCurrentLocation(m_pOauth2->accessToken());
}

QString Form::convertMs2String(qlonglong ms)
{
    QDateTime dt;
    dt.setMSecsSinceEpoch(ms);
    return dt.toString("dd MMM yyyy hh:mm");
}

void Form::onLoadFinished(bool ok)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << ok;

    if (ok) {
        disconnect(ui->webView, SIGNAL(loadFinished(bool)),this, SLOT(onLoadFinished(bool)));
        m_pageLoaded = true;
        if (m_loginCompleted) {
            qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "begin get current location...";
            m_pManager->getCurrentLocation(m_pOauth2->accessToken());
        }
    }
}

void Form::gotoLocation(const QVariant& location)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;
    qlonglong t = 0;
    int zoom = ui->cbZoom->currentText().toInt();
    QString type = ui->cbMapType->currentText();

    m_location = location;
    if (location.toMap().find("timestampMs") != location.toMap().end()) {
        t = location.toMap()["timestampMs"].toLongLong();
        qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "Found datetime t=" << t;
    } else {
        qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "No datetime present...";
    }

    ui->webView->page()->currentFrame()->documentElement().evaluateJavaScript(
                QString("var myLatLng = new google.maps.LatLng(%1, %2);").arg(location.toMap()["latitude"].toDouble()).arg(location.toMap()["longitude"].toDouble())+
                QString("var myOptions = {") +
                QString("center: myLatLng,")+
                QString("zoom: %1,").arg(zoom) +
                QString("mapTypeId: google.maps.MapTypeId.%1,").arg(type) +
                QString("panControl: true") +
                QString("};") +
                QString("var map = new google.maps.Map(document.getElementById(\"map_canvas\"), myOptions);")+
                QString("var marker = new google.maps.Marker({") +
                QString("position: myLatLng,") +
                QString("map: map,") +
                QString("title: \"%1\"});").arg(convertMs2String(t)) +
                QString("var contentString = '<div id=\"content\">Latitude: %1<br>Longitude: %2</div>';").arg(location.toMap()["latitude"].toString()).arg(location.toMap()["longitude"].toString()) +
                QString("var infowindow = new google.maps.InfoWindow({ content: contentString });") +
                QString("google.maps.event.addListener(marker, 'click', function() {") +
                QString("infowindow.open(map,marker);});")
                );
}

void Form::onCurrentLocationReady()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    const QVariant& location = m_pManager->currentLocation();
    gotoLocation(location);
    m_pManager->getLocationHistory(m_pOauth2->accessToken());
}

void Form::getHistoryLocation()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    m_pManager->getLocationHistory(m_pOauth2->accessToken());
}

void Form::onLocationHistoryReady()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;
    const QVariantList& history = m_pManager->locationHistory();
    QTableWidgetItem* itemTime;
    QTableWidgetItem* itemLat;
    QTableWidgetItem* itemLng;
    int len = history.size();
    qlonglong ts;

    ui->twHistory->setRowCount(len);
    for (int i=0; i<len; i++) {
        ts = history[i].toMap()["timestampMs"].toLongLong();
        itemTime = new QTableWidgetItem(convertMs2String(ts));
        ui->twHistory->setItem(i,0,itemTime);

        itemLat = new QTableWidgetItem(history[i].toMap()["latitude"].toString());
        ui->twHistory->setItem(i,1,itemLat);

        itemLng = new QTableWidgetItem(history[i].toMap()["longitude"].toString());
        ui->twHistory->setItem(i,2,itemLng);

        ui->twHistory->setRowHeight(i,20);
    }
    ui->twHistory->resizeColumnsToContents();
}

void Form::onAddressLocationsListReady()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    QVariantMap map;
    const QVariantList& list = m_pManager->addressLocationsList();
    int len = list.size();

    if (len == 1) {
        map.insert("longitude",list[0].toMap()["Point"].toMap()["coordinates"].toList()[0]);
        map.insert("latitude",list[0].toMap()["Point"].toMap()["coordinates"].toList()[1]);
        gotoLocation(QVariant(map));
    } else {
        qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << len;
        SelectAddressDlg dlg(this);
        dlg.initData(list);
        if (dlg.exec() == QDialog::Accepted) {
            QVariant var = dlg.getCoordinate();
//            qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__
//                     << "Longitude=" << var.toMap()["Point"].toMap()["coordinates"].toList()[0].toString();

            map.insert("longitude",var.toMap()["Point"].toMap()["coordinates"].toList()[0]);
            map.insert("latitude",var.toMap()["Point"].toMap()["coordinates"].toList()[1]);
            gotoLocation(QVariant(map));
        }
    }
}

void Form::onHistoryCellDoubleClicked(int row,int column)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "row=" << row << ", column=" << column;

    gotoLocation(m_pManager->locationHistory()[row]);
}

void Form::onClickedGo()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;
    if (ui->cbAddress->currentText().isEmpty())
    {
        const QVariant& location = m_pManager->currentLocation();
        gotoLocation(location);
    }
    else
    {
        m_pManager->getLocationsFromAddress(ui->cbAddress->currentText(),m_pOauth2->geocodingKey());
    }
}

void Form::onClickedInsert()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__
             << "longitude=" << m_location.toMap()["longitude"].toString();

    m_pManager->insertLocation(m_location,m_pOauth2->accessToken());
}

void Form::saveSettings()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    m_pSettings->setValue("zoom",ui->cbZoom->currentText());
    m_pSettings->setValue("map_type",ui->cbMapType->currentText());
    m_pSettings->setValue("address",ui->cbAddress->currentText());

    QStringList list;
    int len = ui->cbAddress->count();
    for (int i=0; i<len; i++) {
        list << ui->cbAddress->itemText(i);
    }
    m_pSettings->setValue("list_addresses",list);
}

void Form::twHistoryCustomMenu(const QPoint& point)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;
    QMenu menu(this);
    menu.addAction(m_aDelete);
    menu.addAction(m_aInsertCurrentLocation);
    menu.exec(QCursor::pos());
}

void Form::onLocationInserted()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;
    getHistoryLocation();
}

void Form::onLocationDeleted()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;
    getHistoryLocation();
}

void Form::onDelete()
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;
    int row = ui->twHistory->selectedRanges()[0].topRow();
    m_pManager->deleteLocation(row,m_pOauth2->accessToken());
}

void Form::onInsertCurrentLocation() {
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;
    int row = ui->twHistory->selectedRanges()[0].topRow();
    m_pManager->insertCurrentLocation(row,m_pOauth2->accessToken());
}
