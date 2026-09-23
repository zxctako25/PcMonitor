#pragma once
#include <QFrame>
#include <QString>

class QLabel;

class InfoCard : public QFrame
{
    Q_OBJECT

public:
    explicit InfoCard(const QString& key, QWidget* parent = nullptr);

    void setValue(const QString& value);

private:
    QLabel* m_valueLabel = nullptr;
};
