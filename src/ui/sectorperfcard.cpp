#include <QWidget>
#include <QFrame>
#include <QMap>
#include <QString>

#include "ui/sectorperfcard.h"
#include "core/fmpstructs.h"

SectorPerfCard::SectorPerfCard(QWidget* parent) : QFrame(parent) {
    ui = new Ui::SectorPerfCard();
    ui->setupUi(this);

    labelMap = {
        {"XLK", ui->sTechLabel},
        {"XLC", ui->sCSLabel},
        {"XLY", ui->sConcLabel},
        {"XLP", ui->sConsLabel},
        {"XLE", ui->sEngLabel},
        {"XLF", ui->sFinLabel},
        {"XLV", ui->sHCLabel},
        {"XLI", ui->sIndLabel},
        {"XLB", ui->sMatLabel},
        {"XLRE", ui->sRELabel},
        {"XLU", ui->sUtilLabel}        
    };
    packedSymbols = labelMap.keys().join(",");
}

SectorPerfCard::~SectorPerfCard() {
}

void SectorPerfCard::updateCard(const std::vector<FullQuote>& quotes) {
    qDebug() << "SectorPerfCard: Updating card with " << quotes.size() << " quotes...";
    for (const FullQuote& quote : quotes) {
        if (!labelMap.contains(quote.symbol)) {
            qDebug() << "SectorPerfCard: Symbol not found in label map: " << quote.symbol;
            continue;
        }
        QLabel* label = labelMap[quote.symbol];
        label->setText(QString::number(quote.changePercentage, 'f', 2) + "%");
        if (quote.changePercentage > 0) {
            label->setStyleSheet("color: green;");
        } else if (quote.changePercentage < 0) {
            label->setStyleSheet("color: red;");
        } else {
            label->setStyleSheet("color: black;");
        }
    }
}