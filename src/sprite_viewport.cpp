#include "sprite_viewport.h"
#include "mainwindow.h"
#include "sprite_loader.h"
#include "theme.h"

#include <QPainter>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>

SpriteViewport::SpriteViewport(MainWindow* mainWin, QWidget* parent) : QWidget(parent), m_main(mainWin)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void SpriteViewport::rebuildImages()
{
    m_images.clear();
    SpriteLoader& loader = m_main->loader();
    int total = loader.GetTotalFrameCount();

    for (int i = 0; i < total; i++)
    {
        SpriteFrame* f = loader.GetFrame(i);
        if (!f || f->rgba.empty())
        {
            m_images.emplace_back();
            continue;
        }
        QImage img(f->rgba.data(), f->width, f->height, f->width * 4, QImage::Format_RGBA8888);
        m_images.push_back(img.copy());
    }
}

void SpriteViewport::drawChecker(QPainter& painter, const QRectF& rect)
{
    const float cell = 10.0f; 
    painter.save();
    painter.setClipRect(rect);

    QColor color1(24, 24, 32);
    QColor color2(40, 40, 50);

    int startX = static_cast<int>(rect.left() / cell);
    int startY = static_cast<int>(rect.top() / cell);
    int endX = static_cast<int>(rect.right() / cell) + 1;
    int endY = static_cast<int>(rect.bottom() / cell) + 1;

    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++) 
        {
            QColor c = ((x + y) % 2 == 0) ? color1 : color2;
            painter.fillRect(QRectF(x * cell, y * cell, cell, cell), c);
        }
    }

    painter.restore();
}


void SpriteViewport::drawPlaceholder(QPainter& painter)
{
    QFont titleFont = painter.font();
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(SpriteColors::Accent);
    painter.drawText(rect(), Qt::AlignCenter, "Sprite-Tools");

    QFont subFont = painter.font();
    subFont.setPointSize(12);
    subFont.setBold(false);
    painter.setFont(subFont);
    painter.setPen(SpriteColors::TextDim);

    QRect sub = rect();
    sub.moveTop(sub.top() + 44);
    painter.drawText(sub, Qt::AlignCenter, "Drop .spr file here or press Ctrl+O");
}

void SpriteViewport::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.fillRect(rect(), SpriteColors::BgDark);

    AppState& s = m_main->state();

    if (!s.sprite_loaded || m_images.empty())
    {
        drawPlaceholder(painter);
        return;
    }

    int idx = s.current_frame;
    
    if (idx < 0 || idx >= (int)m_images.size()) 
        return;

    const QImage& img = m_images[idx];
    if (img.isNull()) 
        return;

    float iw = img.width() * s.zoom;
    float ih = img.height() * s.zoom;
    float vw = (float)width();
    float vh = (float)height();

    float maxSx = std::max(0.0f, iw - vw);
    float maxSy = std::max(0.0f, ih - vh);
    s.scroll_x = std::clamp(s.scroll_x, 0.0f, maxSx);
    s.scroll_y = std::clamp(s.scroll_y, 0.0f, maxSy);

    float ox = (vw > iw) ? (vw - iw) * 0.5f : -s.scroll_x;
    float oy = (vh > ih) ? (vh - ih) * 0.5f : -s.scroll_y;

    QRectF sprRect(ox, oy, iw, ih);

    if (s.show_checker)
    {
        QRectF vis = sprRect.intersected(QRectF(0, 0, vw, vh));
        if (vis.isValid())
            drawChecker(painter, vis);
    }

    painter.drawImage(sprRect, img);

    painter.setPen(QPen(SpriteColors::Border, 1.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(sprRect);
}

void SpriteViewport::onSpriteLoaded()  
{ 
    rebuildImages();
    update(); 
}

void SpriteViewport::onSpriteClosed()  
{ 
    m_images.clear(); 
    update(); 
}

void SpriteViewport::onFrameChanged(int) 
{ 
    update(); 
}

void SpriteViewport::onZoomChanged(float) 
{ 
    update(); 
}

void SpriteViewport::wheelEvent(QWheelEvent* event)
{
    float delta = event->angleDelta().y() / 120.0f;
    AppState& s = m_main->state();

    if (event->modifiers() & Qt::ControlModifier)
    {
        float oldZ = s.zoom;
        float newZ = std::clamp(oldZ * (1.0f + delta * 0.15f), 0.25f, 16.0f);
        float r = newZ / oldZ;
        float cw = (float)width(), ch = (float)height();
        s.scroll_x = (s.scroll_x + cw * 0.5f) * r - cw * 0.5f;
        s.scroll_y = (s.scroll_y + ch * 0.5f) * r - ch * 0.5f;
        emit zoomRequested(newZ);
    }
    else if (event->modifiers() & Qt::ShiftModifier)
    {
        s.scroll_x -= delta * 50;
        update();
    }
    else
    {
        s.scroll_y -= delta * 50;
        update();
    }
    event->accept();
}

void SpriteViewport::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_dragging = true;
        m_dragStart = event->pos();
        m_dragScrollX = m_main->state().scroll_x;
        m_dragScrollY = m_main->state().scroll_y;
        setCursor(Qt::ClosedHandCursor);
    }
}

void SpriteViewport::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging)
    {
        QPoint d = event->pos() - m_dragStart;
        m_main->state().scroll_x = m_dragScrollX - d.x();
        m_main->state().scroll_y = m_dragScrollY - d.y();
        update();
    }
}

void SpriteViewport::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton && m_dragging)
    {
        m_dragging = false;
        setCursor(Qt::ArrowCursor);
    }
}