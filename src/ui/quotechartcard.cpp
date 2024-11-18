#include <QFrame>
#include <QWidget>

#include "ui/quotechartcard.h"

QuoteChartCard::QuoteChartCard(QWidget* parent) : QFrame(parent) {
    ui = new Ui::QuoteChartCard();
    ui->setupUi(this);

    connect(ui->submitButton, &QPushButton::clicked, this, &QuoteChartCard::submitButtonClicked);
    connect(ui->symbolInput, &QLineEdit::returnPressed, this, &QuoteChartCard::submitButtonClicked);
}

QuoteChartCard::~QuoteChartCard() {
}

void QuoteChartCard::setSymbol(const QString& sym) {
    qDebug() << "QuoteChartCard: Setting symbol to" << sym;
    symbol = sym;
}

void QuoteChartCard::updateQuote(const FullQuote& quote) {
    if (quote.symbol != symbol) {
        qDebug() << "QuoteChartCard: Symbol mismatch between quote and card!";
        return;
    }

    ui->nameDisplayLabel->setText(quote.name + " (" + quote.exchange + ": " + quote.symbol + ")");
    ui->lastLabel->setText(QString::number(quote.price, 'f', 2));
    ui->chgLabel->setText(QString::number(quote.change, 'f', 2));
    ui->chgPctLabel->setText(QString::number(quote.changePercentage, 'f', 2) + "%");
    if (quote.change > 0) {
        ui->chgLabel->setStyleSheet("color: green;");
        ui->chgPctLabel->setStyleSheet("color: green;");
    } else if (quote.change < 0) {
        ui->chgLabel->setStyleSheet("color: red;");
        ui->chgPctLabel->setStyleSheet("color: red;");
    } else {
        ui->chgLabel->setStyleSheet("color: black;");
        ui->chgPctLabel->setStyleSheet("color: black;");
    }
    ui->openLabel->setText(QString::number(quote.open, 'f', 2));
    ui->dayLowLabel->setText(QString::number(quote.dayLow, 'f', 2));
    ui->dayHighLabel->setText(QString::number(quote.dayHigh, 'f', 2));
    ui->volLabel->setText(QString::number(quote.volume));
    ui->avgVolLabel->setText(QString::number(quote.avgVolume));
    ui->yearHighLabel->setText(QString::number(quote.yearHigh, 'f', 2));
    ui->yearLowLabel->setText(QString::number(quote.yearLow, 'f', 2));
    ui->avg50Label->setText(QString::number(quote.priceAvg50, 'f', 2));
    ui->avg200Label->setText(QString::number(quote.priceAvg200, 'f', 2));
}

void QuoteChartCard::submitButtonClicked() {
    setSymbol(ui->symbolInput->text());
    emit quoteRequested(symbol);
}

