#pragma once

#include "ui_sectorperfcard.h"

#include <vector>

#include <QFrame>
#include <QWidget>
#include <QMap>
#include <QString>
#include <QLabel>

#include "core/fmpstructs.h"

class SectorPerfCard : public QFrame {
    Q_OBJECT

    public:
        explicit SectorPerfCard(QWidget* parent = nullptr);
        ~SectorPerfCard();
        QString packedSymbols;

        void updateCard(const std::vector<FullQuote>& quotes);

    private:
        Ui::SectorPerfCard* ui;
        QMap<QString, QLabel*> labelMap;
};