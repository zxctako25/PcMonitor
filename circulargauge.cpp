#include "circulargauge.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QConicalGradient>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QtMath>

CircularGauge::CircularGauge(QWidget* parent) : QWidget(parent)
{
    setMinimumSize(220, 240);

    m_anim = new QPropertyAnimation(this, "displayValue", this);
    m_anim->setDuration(500);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);
}

void CircularGauge::setTitle(const QString& t)
{
    m_title = t;
    update();
}

void CircularGauge::setSubtitle(const QString& s)
{
    m_subtitle = s;
    update();
}

void CircularGauge::setAccentColor(const QColor& c)
{
    m_accent = c;
    update();
}

void CircularGauge::setTempText(const QString& t)
{
    m_tempText = t;
    update();
}

void CircularGauge::setValue(double v)
{
    v = qBound(0.0, v, 100.0);
    if (qAbs(v - m_value) < 0.01)
        return;

    m_value = v;
    m_anim->stop();
    m_anim->setStartValue(m_displayValue);
    m_anim->setEndValue(m_value);
    m_anim->start();
}

void CircularGauge::setDisplayValue(double v)
{
    m_displayValue = v;
    update();
}

void CircularGauge::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int side   = qMin(width(), height() - 50);
    const int margin = 26;
    QRectF rect((width() - side) / 2.0, margin, side, side);

    p.setPen(QPen(QColor(30, 30, 30), 12, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(rect.adjusted(7, 7, -7, -7), 225 * 16, -270 * 16);

    QConicalGradient grad(rect.center(), -90);
    grad.setColorAt(0.0, m_accent);
    grad.setColorAt(1.0, m_accent.lighter(150));
    p.setPen(QPen(QBrush(grad), 12, Qt::SolidLine, Qt::RoundCap));

    const int span = -static_cast<int>(270 * 16 * (m_displayValue / 100.0));
    p.drawArc(rect.adjusted(7, 7, -7, -7), 225 * 16, span);

    QFont f = font();
    f.setPointSize(26);
    f.setBold(true);
    p.setFont(f);
    p.setPen(Qt::white);

    QRectF centerRect = rect;
    if (!m_tempText.isEmpty())
        centerRect.translate(0, -10);
    p.drawText(centerRect, Qt::AlignCenter,
               QString::number(m_displayValue, 'f', 0) + "%");

    if (!m_tempText.isEmpty())
    {
        QFont ft = font();
        ft.setPointSize(11);
        ft.setBold(false);
        p.setFont(ft);
        p.setPen(QColor(170, 180, 200));

        QRectF tempRect = rect;
        tempRect.translate(0, 30);
        p.drawText(tempRect, Qt::AlignCenter, m_tempText);
    }

    f.setPointSize(11);
    f.setBold(false);
    p.setFont(f);
    p.setPen(QColor(150, 160, 180));
    p.drawText(QRectF(0, 4, width(), 22), Qt::AlignCenter, m_title);

    if (!m_subtitle.isEmpty())
    {
        p.setPen(QColor(120, 130, 150));
        p.drawText(QRectF(0, height() - 26, width(), 22),
                   Qt::AlignCenter, m_subtitle);
    }
}
