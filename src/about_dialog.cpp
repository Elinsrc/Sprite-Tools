#include "about_dialog.h"
#include "theme.h"

#include <QVBoxLayout>
#include <QLabel>

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("About");
    setFixedSize(380, 150);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(18, 18, 18, 18);
    lay->setSpacing(0);

    auto* nameLbl = new QLabel("Sprite-Tools");
    nameLbl->setStyleSheet(QString("font-size: 17px; font-weight: bold; color: %1;").arg(SpriteColors::Accent.name()));
    lay->addWidget(nameLbl);
    lay->addSpacing(6);

    auto* descLbl = new QLabel("Sprite viewer and creator for Quake / Half-Life sprites");
    descLbl->setStyleSheet(QString("font-size: 12px; color: %1;").arg(SpriteColors::TextPrimary.name()));
    descLbl->setWordWrap(true);
    lay->addWidget(descLbl);
    lay->addSpacing(16);


    auto* linkLbl = new QLabel;
    linkLbl->setTextFormat(Qt::RichText);
    linkLbl->setOpenExternalLinks(true);
    linkLbl->setText(
        QString("<span style='color:%1; font-size:12px;'>GitHub: </span>"
                "<a style='color:%2; font-size:12px; text-decoration:underline;' "
                "href='https://github.com/Elinsrc/Sprite-Tools'>"
                "https://github.com/Elinsrc/Sprite-Tools</a>")
            .arg(SpriteColors::TextPrimary.name(), SpriteColors::Accent.name()));
    linkLbl->setCursor(Qt::PointingHandCursor);
    lay->addWidget(linkLbl);

    lay->addStretch();
}