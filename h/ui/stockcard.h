#pragma once

#include "ui_stockcard.h"

#include <QFrame>

#include "core/fmpstructs.h"


class StockCard : public QFrame {
    Q_OBJECT

    public:
        explicit StockCard(QWidget* parent = nullptr);
        ~StockCard();

        void updateQuote(const FullQuote& quote);
        void updatePerformance(const StockPriceChange& perf);
        void setSymbol(const QString& symbol) { this->symbol = symbol; }
        QString getSymbol() { return symbol; }

    private:
        Ui::StockCard* ui;
        QString symbol = "";
};