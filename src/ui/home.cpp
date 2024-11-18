
#include <QWidget>
#include <QTimer>
#include <QMap>
#include <QtCharts>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QChart>
#include <QLineSeries>
#include <QChartView>
#include <QDebug>

#include "ui/home.h"

Home::Home(QWidget* parent) : QWidget(parent) {
    qDebug() << "Home: Initializing Home widget...";
    ui = new Ui::Home();
    ui->setupUi(this);
    ui->horizontalLayout_2->setProperty("transparent", true); // sets bottom layout tohave no element frame stlying
    if (!ui->chartWidget->layout()) {
        QVBoxLayout* layout = new QVBoxLayout(ui->chartWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        ui->chartWidget->setLayout(layout);
    }
    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(1000);
    marketChecker = new MarketHoursChecker(this);
    tradingHours = marketChecker->isMarketOpen();
    fmp = new FMP(this);

    stockCards = {
        {"^IXIC", ui->ixicCard},
        {"^DJI", ui->djiCard},
        {"^GSPC", ui->gspcCard}
    };
    // set card symbols
    for (const QString& sym : stockCards.keys()) {
        stockCards[sym]->setSymbol(sym);
    }

    connect(refreshTimer, &QTimer::timeout, this, &Home::onRefreshTimeout);
    connect(marketChecker, &MarketHoursChecker::marketStatusChanged, this, &Home::onMarketStatusChanged);
    connect(fmp, &FMP::stockQuoteReceived, this, &Home::onStockQuoteReceived);
    connect(fmp, &FMP::stockPerformanceReceived, this, &Home::onStockPerformanceReceived);
    connect(fmp, &FMP::historicalDailyReceived, this, &Home::onHistoricalDailyReceived);

    connect(ui->chartDjiButton, &QPushButton::clicked, this, &Home::onChartDjiButtonClicked);
    connect(ui->chartSpButton, &QPushButton::clicked, this, &Home::onChartGspcButtonClicked);
    connect(ui->chartNdxButton, &QPushButton::clicked, this, &Home::onChartIxicButtonClicked);

    initHome();
}

Home::~Home() {
}

void Home::initHome() {
    QString syms = stockCards.keys().join(",");
    fmp->getStockPerformance(syms, "home");
    fmp->getHistoricalDaily(syms, "home.chart", QDateTime::currentDateTime().addDays(-30));
    syms += "," + ui->sectorCard->packedSymbols;
    fmp->getStockQuote(syms, "home.init");
    if (tradingHours) {
        ui->marketStatusLabel->setText("Open");
        ui->marketStatusLabel->setStyleSheet("color: green;");
        if (!refreshTimer->isActive()) {
            refreshTimer->start();
        }
    } else if (!tradingHours) {
        ui->marketStatusLabel->setText("Closed");
        ui->marketStatusLabel->setStyleSheet("color: red;");
        if (refreshTimer->isActive()) {
            refreshTimer->stop();
        }
    }
}

void Home::refreshData() {
    qDebug() << "Home: Refreshing data...";
    QString syms = stockCards.keys().join(",");
    // join with sector symbols
    syms += "," + ui->sectorCard->packedSymbols;
    fmp->getStockQuote(syms, "home");
}

void Home::updateChart(const StockHistoricalEodData& data) {
    qDebug() << "Home: Updating chart with data for symbol:" << data.symbol;
    
    // Create a new line series with optimized appearance
    QLineSeries* series = new QLineSeries();
    series->setName(data.symbol);
    series->setPen(QPen(QColor("#00ff00"), 1.5)); // Brighter green, slightly thicker line
    
    // Get the data points
    QList<QPointF> points = data.getChartPoints();
    series->replace(points);
    
    // Create and configure chart
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("%1 Price History").arg(data.symbol));
    chart->setAnimationOptions(QChart::NoAnimation); // Disable animations for better performance
    chart->setBackgroundVisible(false); // Remove chart background
    chart->setMargins(QMargins(0, 0, 0, 0)); // Remove margins around chart
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    
    // Configure axes
    QDateTimeAxis* axisX = new QDateTimeAxis;
    axisX->setTickCount(6); // Reduce number of labels for better spacing
    axisX->setFormat("MM/dd"); // Shorter date format
    axisX->setLabelsColor(Qt::white);
    axisX->setGridLineVisible(false); // Remove grid lines for cleaner look
    
    QValueAxis* axisY = new QValueAxis;
    axisY->setTickCount(5); // Reduce number of labels
    axisY->setLabelFormat("%.0f"); // Remove decimal places to save space
    axisY->setLabelsColor(Qt::white);
    axisY->setGridLineVisible(false);
    axisY->setMinorGridLineVisible(false);
    
    // Get ranges with padding
    QPair<QDateTime, QDateTime> dateRange = data.getDateRange();
    axisX->setRange(dateRange.first, dateRange.second);
    
    QPair<double, double> priceRange = data.getPriceRange();
    double padding = (priceRange.second - priceRange.first) * 0.02; // Reduce padding to 2%
    axisY->setRange(priceRange.first - padding, priceRange.second + padding);
    
    // Attach axes
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    
    // Configure legend
    chart->legend()->setVisible(false); // Hide legend to save space
    
    // Update chart view
    if (!chartView) {
        chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        
        // Set the chart view to use all available space
        chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        
        // Add to layout if not already added
        if (!ui->chartWidget->layout()) {
            QVBoxLayout* layout = new QVBoxLayout(ui->chartWidget);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);
            ui->chartWidget->setLayout(layout);
        }
        ui->chartWidget->layout()->addWidget(chartView);
    } else {
        chartView->setChart(chart);
    }
    
    // Set minimum size for the chart view
    chartView->setMinimumSize(300, 150); // Adjust these values based on your needs
}


void Home::onRefreshTimeout() {
    qDebug() << "Home: Refresh timer timeout...";
    refreshData();
}

void Home::onStockQuoteReceived(const std::vector<FullQuote>& quote, FmpRequest request) {
    qDebug() << "Home: Received stock quote data..." << request.requestor;
    if (request.requestor.split(".").at(0) != "home") {
        qDebug() << "Home: Requestor not recognized, skipping...";
        return;
    }

    if (request.requestor == "home.init") {
        qDebug() << "Home: Updating sector card...";
        ui->sectorCard->updateCard(quote);
    }
    
    for (const FullQuote& q : quote) {
        if (!stockCards.contains(q.symbol)) {
            qDebug() << "Home: Symbol" << q.symbol << "not found in stockCards map, skipping...";
            continue;
        }
        StockCard* card = stockCards[q.symbol];
        card->updateQuote(q);
    }
}

void Home::onStockPerformanceReceived(const std::vector<StockPriceChange>& perf, FmpRequest request) {
    qDebug() << "Home: Received stock performance data...";
    if (request.requestor != "home") {
        return;
    }

    for (const StockPriceChange& p : perf) {
        StockCard* card = stockCards[p.symbol];
        card->updatePerformance(p);
    }
}

void Home::onHistoricalDailyReceived(const std::vector<StockHistoricalEodData>& data, FmpRequest request) {
    qDebug() << "Home: Received historical daily data...";
    if (request.requestor != "home.chart") {
        return;
    }

    for (const StockHistoricalEodData& d : data) {
        historicalCache[d.symbol] = d;
    }

    if (!chartIsInit) {
        if (historicalCache.contains(stockCards.keys().at(0))) {
            updateChart(historicalCache[stockCards.keys().at(0)]);
            chartIsInit = true;
        } else {
            qDebug() << "Home: Historical data not found in cache.";
        }

    }
}

void Home::onMarketStatusChanged(bool isOpen) {
    qDebug() << "Home: Market status changed...";
    tradingHours = isOpen;
    if (tradingHours) {
        ui->marketStatusLabel->setText("Open");
        ui->marketStatusLabel->setStyleSheet("color: green;");
        if (!refreshTimer->isActive()) {
            refreshTimer->start();
        }
    } else if (!tradingHours) {
        ui->marketStatusLabel->setText("Closed");
        ui->marketStatusLabel->setStyleSheet("color: red;");
        if (refreshTimer->isActive()) {
            refreshTimer->stop();
        }
    }
}

void Home::onChartDjiButtonClicked() {
    qDebug() << "Home: DJI chart button clicked...";
    if (historicalCache.contains("^DJI")) {
        updateChart(historicalCache["^DJI"]);
    } else {
        qDebug() << "Home: Historical data for DJI not found in cache.";
    }
}

void Home::onChartGspcButtonClicked() {
    qDebug() << "Home: GSPC chart button clicked...";
    if (historicalCache.contains("^GSPC")) {
        updateChart(historicalCache["^GSPC"]);
    } else {
        qDebug() << "Home: Historical data for GSPC not found in cache.";
    }
}

void Home::onChartIxicButtonClicked() {
    qDebug() << "Home: IXIC chart button clicked...";
    if (historicalCache.contains("^IXIC")) {
        updateChart(historicalCache["^IXIC"]);
    } else {
        qDebug() << "Home: Historical data for IXIC not found in cache.";
    }
}