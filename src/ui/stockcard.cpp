#include <QFrame>

#include "ui/stockcard.h"
#include "core/fmpstructs.h"

StockCard::StockCard(QWidget* parent) : QFrame(parent) {
    ui = new Ui::StockCard();
    ui->setupUi(this);
}

StockCard::~StockCard() {
}


void StockCard::updateQuote(const FullQuote& quote) {
    if (quote.symbol != symbol) {
        qDebug() << "StockCard: Symbol mismatch between quote and card!";
        return;
    }

    ui->nameLabel->setText(quote.name);
    if (quote.change > 0) {
        ui->priceLabel->setStyleSheet("color: green;");
        ui->priceChangeLabel->setStyleSheet("color: green;");
        ui->priceChangePctLabel->setStyleSheet("color: green;");
    } else if (quote.change < 0) {
        ui->priceLabel->setStyleSheet("color: red;");
        ui->priceChangeLabel->setStyleSheet("color: red;");
        ui->priceChangePctLabel->setStyleSheet("color: red;");
    } else {
        ui->priceLabel->setStyleSheet("color: black;");
        ui->priceChangeLabel->setStyleSheet("color: black;");
        ui->priceChangePctLabel->setStyleSheet("color: black;");
    }
    ui->priceLabel->setText(QString::number(quote.price, 'f', 2));
    ui->priceChangeLabel->setText(QString::number(quote.change, 'f', 2));
    ui->priceChangePctLabel->setText(QString::number(quote.changePercentage, 'f', 2) + "%");
}

void StockCard::updatePerformance(const StockPriceChange& perf) {
    if (perf.symbol != symbol) {
        qDebug() << "StockCard: Symbol mismatch between performance data and card!";
        return;
    }
    qDebug() << "StockCard: Updating performance data for " << perf.symbol;
    qDebug() << "StockCard: 1D: " << perf.oneDay << "5D" << perf.oneWeek << "1W" << perf.oneWeek << "1M" << perf.oneMonth << "YTD" << perf.ytd << "1Y" << perf.oneYear;
    ui->weekPerfLabel->setText(QString::number(perf.oneWeek, 'f', 2) + "%");
    if (perf.oneWeek > 0) {
        ui->weekPerfLabel->setStyleSheet("color: green;");
    } else if (perf.oneWeek < 0) {
        ui->weekPerfLabel->setStyleSheet("color: red;");
    } else {
        ui->weekPerfLabel->setStyleSheet("color: black;");
    }
    ui->monthPerfLabel->setText(QString::number(perf.oneMonth, 'f', 2) + "%");
    if (perf.oneMonth > 0) {
        ui->monthPerfLabel->setStyleSheet("color: green;");
    } else if (perf.oneMonth < 0) {
        ui->monthPerfLabel->setStyleSheet("color: red;");
    } else {
        ui->monthPerfLabel->setStyleSheet("color: black;");
    }
    ui->ytdPerfLabel->setText(QString::number(perf.ytd, 'f', 2) + "%");
    if (perf.ytd > 0) {
        ui->ytdPerfLabel->setStyleSheet("color: green;");
    } else if (perf.ytd < 0) {
        ui->ytdPerfLabel->setStyleSheet("color: red;");
    } else {
        ui->ytdPerfLabel->setStyleSheet("color: black;");
    }
    ui->yearPerfLabel->setText(QString::number(perf.oneYear, 'f', 2) + "%");
    if (perf.oneYear > 0) {
        ui->yearPerfLabel->setStyleSheet("color: green;");
    } else if (perf.oneYear < 0) {
        ui->yearPerfLabel->setStyleSheet("color: red;");
    } else {
        ui->yearPerfLabel->setStyleSheet("color: black;");
    }
}