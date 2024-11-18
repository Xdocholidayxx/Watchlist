#pragma once

#include "ui_quotechartcard.h"

#include <QFrame>
#include <QWidget>

#include "core/fmpstructs.h"


class QuoteChartCard : public QFrame {
    Q_OBJECT

    public:
        QuoteChartCard(QWidget* parent = nullptr);
        ~QuoteChartCard();

        void setSymbol(const QString& sym);
        void updateQuote(const FullQuote& quote);
        //void updateChart();

    signals:
        void quoteRequested(const QString& symbol);

    public slots:
        void submitButtonClicked();

    private:
        Ui::QuoteChartCard* ui;
        QString symbol;

};