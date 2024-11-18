#pragma once

#include <vector>

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QHash>

#include "core/config.h"
#include "core/fmpstructs.h"

enum class FmpRequestType {
    STOCK_QUOTE,
    STOCK_PERFORMANCE,
    HISTORICAL_DAILY
};

struct FmpRequest {
    FmpRequestType type;
    QString requestor;
};
Q_DECLARE_METATYPE(FmpRequest)

class FMP : public QObject {
    Q_OBJECT

    public:
        explicit FMP(QObject* parent = nullptr);
        ~FMP();

        void getStockQuote(const QString& symbol, const QString& requestor);
        void getStockPerformance(const QString& symbol, const QString& requestor);
        void getHistoricalDaily(const QString& symbol, const QString& requestor, const QDateTime& start = QDateTime(), const QDateTime& end = QDateTime());
        //void getHistoricalIntraday(const QString& symbol, const QString& requestor);

    signals:
        void stockQuoteReceived(const std::vector<FullQuote>& quote, FmpRequest request);
        void stockPerformanceReceived(const std::vector<StockPriceChange>& perf, FmpRequest request);
        void historicalDailyReceived(const std::vector<StockHistoricalEodData>& data, FmpRequest request);

    private slots:
        void onNetworkReply(QNetworkReply* reply);

    private:
        QNetworkAccessManager* _networkManager;
        QString _fmpKey = Config::getInstance().get("fmpKey", true);

        void callApi(const QUrl& endpoint, const FmpRequest request);

        void parseStockQuotes(const QJsonArray& arr, const FmpRequest& request);
        void parseStockPerformance(const QJsonArray& arr, const FmpRequest& request);
        void parseHistoricalDailySingle(const QJsonObject& obj, const FmpRequest& request);
        void parseHistoricalDailyMulti(const QJsonArray& arr, const FmpRequest& request);
};