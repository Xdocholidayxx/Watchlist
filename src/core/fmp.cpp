
#include <vector>

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QHash>
#include <QVariant>
#include <QDebug>

#include "core/fmp.h"


FMP::FMP(QObject* parent) : QObject(parent) {
    qDebug() << "FMP: Initializing FMP object...";
    _networkManager = new QNetworkAccessManager(this);
    connect(_networkManager, &QNetworkAccessManager::finished, this, &FMP::onNetworkReply);
}

FMP::~FMP() {
    delete _networkManager;
}

void FMP::getStockQuote(const QString& symbol, const QString& requestor) {
    QUrl url("https://financialmodelingprep.com/api/v3/quote/" + symbol + "?apikey=" + _fmpKey);
    FmpRequest request = {FmpRequestType::STOCK_QUOTE, requestor};
    callApi(url, request);
}

void FMP::getStockPerformance(const QString& symbol, const QString& requestor) {
    QUrl url("https://financialmodelingprep.com/api/v3/stock-price-change/" + symbol + "?apikey=" + _fmpKey);
    FmpRequest request = {FmpRequestType::STOCK_PERFORMANCE, requestor};
    callApi(url, request);
}

void FMP::getHistoricalDaily(const QString& symbol, const QString& requestor, const QDateTime& start, const QDateTime& end) {
    QUrl url("https://financialmodelingprep.com/api/v3/historical-price-full/" + symbol);
    QUrlQuery query;
    if (start.isValid()) {
        query.addQueryItem("from", start.toString(Qt::ISODate));
    }
    if (end.isValid()) {
        query.addQueryItem("to", end.toString(Qt::ISODate));
    }
    query.addQueryItem("apikey", _fmpKey);
    url.setQuery(query);

    FmpRequest request = {FmpRequestType::HISTORICAL_DAILY, requestor};
    callApi(url, request);
}


void FMP::callApi(const QUrl& endpoint, const FmpRequest request) {
    QNetworkRequest r;
    r.setUrl(endpoint);
    r.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    r.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), 
                        QVariant::fromValue(request));
    
    _networkManager->get(r);

    qDebug() << "FMP: Sent request to " << endpoint;
}

void FMP::onNetworkReply(QNetworkReply* reply) {
    qDebug() << "FMP: Received reply from " << reply->url();
    FmpRequest request = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1)).value<FmpRequest>();
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray arr;
    QJsonArray hist;
    QJsonObject rObj;
    switch (request.type) {
        case FmpRequestType::STOCK_QUOTE: {
            arr = doc.array();
            parseStockQuotes(arr, request);
            break;
        case FmpRequestType::STOCK_PERFORMANCE:
            arr = doc.array();
            parseStockPerformance(arr, request);
            break;
        case FmpRequestType::HISTORICAL_DAILY:
            rObj = doc.object();
            if (rObj.contains("historicalStockList")) {
                arr = rObj["historicalStockList"].toArray();
                parseHistoricalDailyMulti(arr, request);
            } else {
                parseHistoricalDailySingle(rObj, request);
            }
            break;
        default:
            break;
        }
    }
}

void FMP::parseStockQuotes(const QJsonArray& arr, const FmpRequest& request) {
    std::vector<FullQuote> quotes;
    for (const auto& obj : arr) {
        QJsonObject o = obj.toObject();
        FullQuote q(o);
        quotes.push_back(q);

        qDebug() << "FMP: Received quote for " << q.symbol;
    }
    emit stockQuoteReceived(quotes, request);
}

void FMP::parseStockPerformance(const QJsonArray& arr, const FmpRequest& request) {
    std::vector<StockPriceChange> perf;
    for (const auto& obj : arr) {
        QJsonObject o = obj.toObject();
        StockPriceChange p(o);
        perf.push_back(p);

        qDebug() << "FMP: Received performance data for " << p.symbol;
    }
    emit stockPerformanceReceived(perf, request);
}

void FMP::parseHistoricalDailySingle(const QJsonObject& obj, const FmpRequest& request) {
    std::vector<StockHistoricalEodData> outvec;
    StockHistoricalEodData data(obj);
    outvec.push_back(data);
    emit historicalDailyReceived(outvec, request);
}

void FMP::parseHistoricalDailyMulti(const QJsonArray& arr, const FmpRequest& request) {
    std::vector<StockHistoricalEodData> outvec;
    for (const auto& obj : arr) {
        QJsonObject o = obj.toObject();
        StockHistoricalEodData data(o);
        outvec.push_back(data);
    }
    emit historicalDailyReceived(outvec, request);
}