#pragma once

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <vector>

class MainWindow;

class SpriteViewport : public QWidget
{
    Q_OBJECT

public:
    explicit SpriteViewport(MainWindow* mainWin, QWidget* parent = nullptr);
    ~SpriteViewport() = default;

    void deleteTextures() { m_images.clear(); }

signals:
    void zoomRequested(float newZoom);

public slots:
    void onSpriteLoaded();
    void onSpriteClosed();
    void onFrameChanged(int frame);
    void onZoomChanged(float zoom);

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void rebuildImages();
    void drawChecker(QPainter& painter, const QRectF& rect);
    void drawPlaceholder(QPainter& painter);

    MainWindow* m_main;
    std::vector<QImage> m_images;

    bool m_dragging = false;
    QPoint m_dragStart;
    float m_dragScrollX = 0;
    float m_dragScrollY = 0;
};