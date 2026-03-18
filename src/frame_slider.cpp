#include "frame_slider.h"
#include "theme.h"

#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>

FrameSlider::FrameSlider(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(28);
    setMaximumHeight(28);
    setMinimumWidth(80);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMouseTracking(true);
}

void FrameSlider::setRange(int min, int max)
{
    m_min = min;
    m_max = std::max(min, max);
    m_value = std::clamp(m_value, m_min, m_max);
    update();
}

void FrameSlider::setValue(int value)
{
    int clamped = std::clamp(value, m_min, m_max);
    if (clamped != m_value)
    {
        m_value = clamped;
        update();
        emit valueChanged(m_value);
    }
}

void FrameSlider::setEnabled(bool enabled)
{
    m_enabled = enabled;
    QWidget::setEnabled(enabled);
    update();
}

int FrameSlider::posToValue(int x) const
{
    if (m_max <= m_min) return m_min;
    const int margin = 10;
    int trackW = width() - margin * 2;
    if (trackW <= 0) return m_min;
    float ratio = std::clamp((float)(x - margin) / (float)trackW, 0.0f, 1.0f);
    return m_min + (int)std::round(ratio * (m_max - m_min));
}

int FrameSlider::valueToPos(int val) const
{
    if (m_max <= m_min) return width() / 2;
    const int margin = 10;
    int trackW = width() - margin * 2;
    float ratio = (float)(val - m_min) / (float)(m_max - m_min);
    return margin + (int)(ratio * trackW);
}

void FrameSlider::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int cy = height() / 2;
    const int margin = 10;
    int trackW = width() - margin * 2;
    int steps = m_max - m_min;

    QColor trackBg = m_enabled ? SpriteColors::BgLight : SpriteColors::BgDark;
    p.setPen(Qt::NoPen);
    p.setBrush(trackBg);
    p.drawRoundedRect(margin, cy - 2, trackW, 4, 2, 2);

    if (!m_enabled || steps <= 0)
        return;

    int thumbX = valueToPos(m_value);
    int activeW = thumbX - margin;
    if (activeW > 0)
    {
        p.setBrush(SpriteColors::AccentDim);
        p.drawRoundedRect(margin, cy - 2, activeW, 4, 2, 2);
    }

    if (steps > 0 && steps <= 60)
    {
        float dotR = (steps <= 20) ? 3.0f : (steps <= 40) ? 2.0f : 1.5f;

        for (int i = 0; i <= steps; i++)
        {
            int dx = valueToPos(m_min + i);
            bool active = (m_min + i) <= m_value;

            QColor dc = active ? SpriteColors::Accent : SpriteColors::TextDim;
            dc.setAlpha(active ? 200 : 100);

            p.setBrush(dc);
            p.drawEllipse(QPointF(dx, cy), dotR, dotR);
        }
    }

    p.setBrush(QColor(0, 0, 0, 40));
    p.drawEllipse(QPointF(thumbX, cy + 1), 8, 8);

    p.setBrush(SpriteColors::Accent);
    p.drawEllipse(QPointF(thumbX, cy), 7, 7);
}

void FrameSlider::mousePressEvent(QMouseEvent* event)
{
    if (!m_enabled || m_max <= m_min) return;
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = true;
        int v = posToValue(event->pos().x());
        if (v != m_value)
        {
            m_value = v;
            update();
            emit valueChanged(m_value);
        }
    }
}

void FrameSlider::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_dragging || !m_enabled) return;
    int v = posToValue(event->pos().x());
    if (v != m_value)
    {
        m_value = v;
        update();
        emit valueChanged(m_value);
    }
}

void FrameSlider::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        m_dragging = false;
}