#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>
#include <QJson/Parser>

#include "latitude_data_manager.h"

LatitudeDataManager::LatitudeDataManager(QObject *parent) :
    QObject(parent)
{
    m_pNetworkAccessManager = new QNetworkAccessManager(this);

    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

LatitudeDataManager::~LatitudeDataManager()
{
    delete m_pNetworkAccessManager;
}

void LatitudeDataManager::getCurrentLocation(const QString& access_token)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    QString query = QString("https://www.googleapis.com/latitude/v1/currentLocation?access_token=%1")
            .arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(query)));
}

void LatitudeDataManager::getLocationHistory(const QString& access_token)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    QString query = QString("https://www.googleapis.com/latitude/v1/location"
                            "?granularity=best"
                            "&max-results=10"
                            "&access_token=%1")
            .arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(query)));
}

void LatitudeDataManager::getLocationsFromAddress(const QString& address, const QString& apiKey)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    QString s = address;
    s.replace(" ","+");

    QString query = QString("http://maps.google.com/maps/geo?q=%1&key=%2&output=json&oe=utf8&sensor=false").arg(s).arg(apiKey);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(query)));
}

void LatitudeDataManager::insertLocation(const QVariant& location, const QString& access_token)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__;

    qlonglong t;
    QString longitude = location.toMap()["longitude"].toString();
    QString latitude = location.toMap()["latitude"].toString();
    if (location.toMap().find("timestampMs") != location.toMap().end()) {
        t = location.toMap()["timestampMs"].toLongLong();
        qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "Found time data=" << t;
    } else {
        t = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "No time data, use current time=" << t;
    }
    if (t == 0) {
        t = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "Change time data to current=" << t;
    }

    QString query = QString("https://www.googleapis.com/latitude/v1/location"
                            "?access_token=%1").arg(access_token);
    QByteArray params;
    QNetworkRequest request;
    request.setUrl(QUrl(query));
    request.setRawHeader("Content-Type", "application/json");
    QString s =
        "{"
        "\"data\":{"
        "\"kind\":\"latitude#location\","
        "\"timestampMs\":\""+QString::number(t)+"\","
        "\"latitude\":"+latitude+","
        "\"longitude\":"+longitude+
        "}"
        "}";
    params.append(s);
    m_pNetworkAccessManager->post(request, params);
}

void LatitudeDataManager::deleteLocation(int row, const QString& access_token)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "row=" << row;
    QVariant location = m_locationHistory[row];
    QString longitude = location.toMap()["longitude"].toString();
    QString latitude = location.toMap()["latitude"].toString();
    qlonglong t = location.toMap()["timestampMs"].toLongLong();

    QString query = QString("https://www.googleapis.com/latitude/v1/location/"+QString::number(t)+
                            "?access_token=%1").arg(access_token);
    m_pNetworkAccessManager->deleteResource(QNetworkRequest(QUrl(query)));
}

void LatitudeDataManager::insertCurrentLocation(int row, const QString& access_token)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << "row=" << row;

    QVariant location = m_locationHistory[row];
    QString longitude = location.toMap()["longitude"].toString();
    QString latitude = location.toMap()["latitude"].toString();

    QString query = QString("https://www.googleapis.com/latitude/v1/currentLocation/"
                            "?access_token=%1").arg(access_token);
    QByteArray params;
    QNetworkRequest request;
    request.setUrl(QUrl(query));
    request.setRawHeader("Content-Type", "application/json");
    QString s =
        "{"
        "\"data\":{"
        "\"kind\":\"latitude#location\","
        "\"latitude\":"+latitude+","
        "\"longitude\":"+longitude+
        "}"
        "}";
    params.append(s);
    m_pNetworkAccessManager->post(request, params);
}

void LatitudeDataManager::replyFinished(QNetworkReply *reply)
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<  __FUNCTION__ << reply->url();

    QString url = reply->url().toString();
    QString json = reply->readAll();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//    qDebug() << "****************###, HTTP status code=" << statusCode << ", Reply = " << json;
//    qDebug() << "****************###, url=" << url;

    if (statusCode == 204) {
        if (reply->operation() == QNetworkAccessManager::DeleteOperation) {
            emit locationDeleted();
            return; //delete operation is OK
        }
    }

    if (json.length() == 0) {
        QMessageBox(QMessageBox::Information,"Info","No data present.",QMessageBox::Ok,QApplication::activeWindow()).exec();
        return;
    }

    QJson::Parser parser;

    bool ok;

    // json is a QString containing the data to convert
    QVariant result = parser.parse (json.toLatin1(), &ok);
    if (!ok) {
        emit errorOccured(QString("Cannot convert to QJson object: %1").arg(json));
        return;
    }

    if (result.toMap().contains("error")) {
        emit errorOccured("Server error: "+result.toMap()["error"].toMap()["message"].toString());
        return;
    }

    if (result.toMap()["data"].toMap()["kind"] == "latitude#location") {
        if (url.contains("currentLocation")) {
            m_currentLocation = result.toMap()["data"];
            emit currentLocationReady();
        } else {
            if (reply->operation() == QNetworkAccessManager::PostOperation) {
                emit locationInserted();
            }
        }
        return;
    } else if (result.toMap()["data"].toMap()["kind"] == "latitude#locationFeed") {
        m_locationHistory = result.toMap()["data"].toMap()["items"].toList();
        emit locationHistoryReady();
        return;
    } else if (result.toMap()["Status"].toMap()["request"] == "geocode") {
        if (result.toMap()["Status"].toMap()["code"] == "200") {
            m_addressLocationsList = result.toMap()["Placemark"].toList();
        } else {
            m_addressLocationsList.clear();
        }
        emit addressLocationsListReady();
        return;
    }
}
