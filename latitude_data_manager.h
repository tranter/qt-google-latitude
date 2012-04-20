#ifndef LATITUDEDATAMANAGER_H
#define LATITUDEDATAMANAGER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QVariantList>

class LatitudeDataManager : public QObject
{
    Q_OBJECT

public:
    explicit LatitudeDataManager(QObject *parent = 0);
    ~LatitudeDataManager();

    //Requests to Google API service.
    void getCurrentLocation(const QString& access_token);
    void getLocationHistory(const QString& access_token);
    void getLocationsFromAddress(const QString& address, const QString& apiKey);
    void insertLocation(const QVariant& location, const QString& access_token);
    void deleteLocation(int row, const QString& access_token);
    void insertCurrentLocation(int row, const QString& access_token);
    void getUserEmail(const QString& access_token);

    const QVariant& currentLocation() {return m_currentLocation;}
    const QVariantList& locationHistory() {return m_locationHistory;}
    const QVariantList& addressLocationsList() {return m_addressLocationsList;}
    QString userEmail() {return m_strUserEmail;}

signals:
    void errorOccured(const QString& error);
    void currentLocationReady();
    void locationHistoryReady();
    void addressLocationsListReady();
    void locationInserted();
    void locationDeleted();
    void sigUserEmailReady();

private slots:
    void replyFinished(QNetworkReply*);

private:
    QNetworkAccessManager* m_pNetworkAccessManager;
    QVariant m_currentLocation;
    QVariantList m_locationHistory;
    QVariantList m_addressLocationsList;
    QString m_strUserEmail;
};

#endif // LATITUDEDATAMANAGER_H
