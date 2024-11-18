#pragma once

#include <QDateTime>
#include <QTimeZone>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

class MarketHoursChecker : public QObject {
    Q_OBJECT

public:
    enum Market {
        NYSE,
        LSE,
        TSE
    };

    MarketHoursChecker(QObject *parent = nullptr) 
        : QObject(parent)
        , isCurrentlyOpen(false)
        , lastApiCheck(0) {
        qDebug() << "MarketHoursChecker: Initializing MarketHoursChecker...";
        networkManager = new QNetworkAccessManager(this);
        
        // Timer for market status checks - every 5 minutes
        statusCheckTimer = new QTimer(this);
        statusCheckTimer->setInterval(5 * 60 * 1000); // 5 minutes
        connect(statusCheckTimer, &QTimer::timeout,
                this, &MarketHoursChecker::updateMarketStatus);
        
        // Do initial check and start timer
        updateMarketStatus();
        statusCheckTimer->start();
    }

    // Fast cached check - no API calls
    bool isMarketOpen() const {
        return isCurrentlyOpen;
    }

signals:
    void marketStatusChanged(bool isOpen);

private slots:
    void updateMarketStatus() {
        qDebug() << "MarketHoursChecker: Checking market status...";
        
        if (!isPossiblyOpen()) {
            qDebug() << "MarketHoursChecker: Market is not possibly open based on time.";
            updateCachedStatus(false);
            return;
        }

        // Check if we should make an API call
        qint64 now = QDateTime::currentSecsSinceEpoch();
        if (now - lastApiCheck < 60) { // Throttle API calls to once per minute max
            qDebug() << "MarketHoursChecker: Throttling API call.";
            return;
        }
        lastApiCheck = now;

        QString endpoint = "https://financialmodelingprep.com/api/v3/is-the-market-open?exchange=NYSE&apikey=f5f6a691f992b149c627bf6eea3cac5e";
        QNetworkRequest request(endpoint);
        QNetworkReply *reply = networkManager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            bool newStatus = false;
            
            if (reply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject obj = doc.object();
                newStatus = obj["isTheStockMarketOpen"].toBool();
                qDebug() << "MarketHoursChecker: API call successful. Market open status:" << newStatus;
            } else {
                qWarning() << "MarketHoursChecker: Market status API error:" << reply->errorString();
                newStatus = isPossiblyOpen(); // Fallback to time-based check
                qDebug() << "MarketHoursChecker: Fallback to time-based check. Market open status:" << newStatus;
            }

            updateCachedStatus(newStatus);
            reply->deleteLater();
        });
    }

private:
    QNetworkAccessManager *networkManager;
    QTimer *statusCheckTimer;
    bool isCurrentlyOpen;
    qint64 lastApiCheck;

    bool isPossiblyOpen() {
        QDateTime currentTime = QDateTime::currentDateTime();
        QTimeZone etZone("America/New_York");
        QDateTime etTime = currentTime.toTimeZone(etZone);

        int dayOfWeek = etTime.date().dayOfWeek();
        if (dayOfWeek == Qt::Saturday || dayOfWeek == Qt::Sunday) {
            return false;
        }

        QTime timeOfDay = etTime.time();
        int secondsSinceMidnight = timeOfDay.msecsSinceStartOfDay() / 1000;

        const int marketOpen = (9 * 3600) + (30 * 60);
        const int marketClose = 16 * 3600;

        return secondsSinceMidnight >= marketOpen && 
               secondsSinceMidnight < marketClose;
    }

    void updateCachedStatus(bool newStatus) {
        if (newStatus != isCurrentlyOpen) {
            isCurrentlyOpen = newStatus;
            emit marketStatusChanged(isCurrentlyOpen);
        }
    }
};