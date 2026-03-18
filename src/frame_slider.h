#pragma once

#include <QWidget>

class FrameSlider : public QWidget
{
    Q_OBJECT

public:
    explicit FrameSlider(QWidget* parent = nullptr);

    void setRange(int min, int max);
    void setValue(int value);
    int value() const { return m_value; }
    void setEnabled(bool enabled);

signals:
    void valueChanged(int value);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    int posToValue(int x) const;
    int valueToPos(int val) const;

    int m_min = 0;
    int m_max = 0;
    int m_value = 0;
    bool m_dragging = false;
    bool m_enabled = true;
};