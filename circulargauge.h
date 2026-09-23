#pragma once
#include <QWidget>
#include <QColor>
#include <QString>

class QPropertyAnimation;

class CircularGauge : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double displayValue READ displayValue WRITE setDisplayValue)

public:
    explicit CircularGauge(QWidget* parent = nullptr);

    double value() const
    {
        return m_value;
    }

    double displayValue() const
    {
        return m_displayValue;
    }

    void setTitle(const QString& t);
    void setSubtitle(const QString& s);
    void setAccentColor(const QColor& c);
    void setTempText(const QString& t);

public slots:
    void setValue(double v);
    void setDisplayValue(double v);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    double m_value = 0.0;
    double m_displayValue = 0.0;
    QString m_title;
    QString m_subtitle;
    QString m_tempText;
    QColor m_accent = QColor(0, 200, 255);
    QPropertyAnimation* m_anim = nullptr;
};
