#pragma once

#include "ui_home.h"

#include <QWidget>
#include <QTimer>
#include <QMap>
#include <QtCharts>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QChart>
#include <QLineSeries>
#include <QChartView>

#include "core/markethourschecker.h"
#include "core/fmp.h"

class Home : public QWidget {
    Q_OBJECT

    public:
        explicit Home(QWidget* parent = nullptr);
        ~Home();

        void initHome();
        void refreshData();
        void updateChart(const StockHistoricalEodData& data);

    public slots:
        void onRefreshTimeout();
        void onStockQuoteReceived(const std::vector<FullQuote>& quote, FmpRequest request);
        void onStockPerformanceReceived(const std::vector<StockPriceChange>& perf, FmpRequest request);
        void onHistoricalDailyReceived(const std::vector<StockHistoricalEodData>& data, FmpRequest request);
        void onMarketStatusChanged(bool isOpen);

        void onChartDjiButtonClicked();
        void onChartGspcButtonClicked();
        void onChartIxicButtonClicked();

    private:
        Ui::Home* ui;
        QTimer* refreshTimer;
        MarketHoursChecker* marketChecker;
        QMap<QString, StockCard*> stockCards;
        FMP* fmp;
        QMap<QString, StockHistoricalEodData> historicalCache;
        bool chartIsInit = false;
        bool tradingHours = false;
        QChartView* chartView;
};