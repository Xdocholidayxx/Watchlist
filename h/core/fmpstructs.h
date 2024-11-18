#pragma once

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <QPair>

struct FullQuote {
    QString symbol;
    QString name;
    double price;
    double change;
    double changePercentage;
    double open;
    double previousClose;
    double dayLow;
    double dayHigh;
    double yearLow;
    double yearHigh;
    long marketCap;
    long volume;
    long avgVolume;
    double priceAvg50;
    double priceAvg200;
    QString exchange;
    double eps;
    double pe;
    QDateTime earningsAnnouncement;
    long sharesOutstanding;
    QDateTime timestamp;

    FullQuote(const QJsonObject& quote) {
        symbol = quote["symbol"].toString();
        name = quote["name"].toString();
        price = quote["price"].toDouble();
        change = quote["change"].toDouble();
        changePercentage = quote["changesPercentage"].toDouble();
        open = quote["open"].toDouble();
        previousClose = quote["previousClose"].toDouble();
        dayLow = quote["dayLow"].toDouble();
        dayHigh = quote["dayHigh"].toDouble();
        yearLow = quote["yearLow"].toDouble();
        yearHigh = quote["yearHigh"].toDouble();
        marketCap = quote["marketCap"].toDouble();
        volume = quote["volume"].toDouble();
        avgVolume = quote["avgVolume"].toDouble();
        priceAvg50 = quote["priceAvg50"].toDouble();
        priceAvg200 = quote["priceAvg200"].toDouble();
        exchange = quote["exchange"].toString();
        eps = quote["eps"].toDouble();
        pe = quote["pe"].toDouble();
        earningsAnnouncement = QDateTime::fromString(quote["earningsAnnouncement"].toString(), Qt::ISODateWithMs);
        sharesOutstanding = quote["sharesOutstanding"].toDouble();
        timestamp = QDateTime::fromSecsSinceEpoch(quote["timestamp"].toDouble());
    }
};

struct StockPriceChange {
    QString symbol;
    double oneDay;
    double oneWeek;
    double oneMonth;
    double threeMonth;
    double sixMonth;
    double ytd;
    double oneYear;
    double threeYear;
    double fiveYear;
    double tenYear;
    double max;

    StockPriceChange(const QJsonObject& change) {
        symbol = change["symbol"].toString();
        oneDay = change["1D"].toDouble();
        oneWeek = change["5D"].toDouble();
        oneMonth = change["1M"].toDouble();
        threeMonth = change["3M"].toDouble();
        sixMonth = change["6M"].toDouble();
        ytd = change["ytd"].toDouble();
        oneYear = change["1Y"].toDouble();
        threeYear = change["3Y"].toDouble();
        fiveYear = change["5Y"].toDouble();
        tenYear = change["10Y"].toDouble();
        max = change["max"].toDouble();
    };
};


struct HistoricalEodPrice {
    QDateTime date;
    double open;
    double high;
    double low;
    double close;
    double adjClose;
    long volume;
    long unadjustedVolume;
    double change;
    double changePercent;
    double vwap;
    QString label;
    double changeOverTime;

    HistoricalEodPrice(const QJsonObject& data) {
        date = QDateTime::fromString(data["date"].toString(), Qt::ISODate);
        open = data["open"].toDouble();
        high = data["high"].toDouble();
        low = data["low"].toDouble();
        close = data["close"].toDouble();
        adjClose = data["adjClose"].toDouble();
        volume = data["volume"].toDouble();
        unadjustedVolume = data["unadjustedVolume"].toDouble();
        change = data["change"].toDouble();
        changePercent = data["changePercent"].toDouble();
        vwap = data["vwap"].toDouble();
        label = data["label"].toString();
        changeOverTime = data["changeOverTime"].toDouble();
    };

    HistoricalEodPrice() {};
};


struct StockHistoricalEodData {
    QString symbol;
    std::vector<HistoricalEodPrice> data;

    StockHistoricalEodData() = default;

    StockHistoricalEodData(const QJsonObject& data) {
        symbol = data["symbol"].toString();
        QJsonArray arr = data["historical"].toArray();
        for (const auto& obj : arr) {
            HistoricalEodPrice price(obj.toObject());
            this->data.push_back(price);
        }
    };

    // Returns data formatted for QtCharts
    QList<QPointF> getChartPoints() const {
        QList<QPointF> points;
        points.reserve(data.size());

        // Convert datetime to msecs since epoch for x-axis
        for (auto it = data.rbegin(); it != data.rend(); ++it) {
            qreal x = it->date.toMSecsSinceEpoch();
            qreal y = it->close;
            points.append(QPointF(x, y));
        }

        return points;
    }

    // Get min/max values for helping set up axes
    QPair<double, double> getPriceRange() const {
        double minPrice = std::numeric_limits<double>::max();
        double maxPrice = std::numeric_limits<double>::lowest();
        
        for (const auto& price : data) {
            minPrice = std::min(minPrice, price.low);
            maxPrice = std::max(maxPrice, price.high);
        }

        return {minPrice, maxPrice};
    }

    // Get date range for axis setup
    QPair<QDateTime, QDateTime> getDateRange() const {
        if (data.empty()) {
            return {QDateTime(), QDateTime()};
        }
        
        // Remember data is stored newest first
        return {
            data.back().date,   // Earliest date
            data.front().date   // Latest date
        };
    }
};