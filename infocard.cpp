#include "infocard.h"
#include <QLabel>
#include <QHBoxLayout>

InfoCard::InfoCard(const QString& key, QWidget* parent) : QFrame(parent)
{
    setStyleSheet("InfoCard{background:transparent; border:none;}");
    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 6, 0, 6);
    lay->setSpacing(12);

    auto* k = new QLabel(key, this);
    k->setStyleSheet("color:#6c7485; font-size:12px;");
    k->setFixedWidth(110);

    m_valueLabel = new QLabel("...", this);
    m_valueLabel->setStyleSheet("color:#e0e6f0; font-size:13px; font-weight:500;");
    m_valueLabel->setWordWrap(true);

    lay->addWidget(k);
    lay->addWidget(m_valueLabel, 1);
}

void InfoCard::setValue(const QString& value)
{
    m_valueLabel->setText(value);
}
