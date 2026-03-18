#pragma once

#include <QScrollArea>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>

class MainWindow;

class PropertiesPanel : public QScrollArea
{
    Q_OBJECT

public:
    explicit PropertiesPanel(MainWindow* mainWin, QWidget* parent = nullptr);

public slots:
    void onSpriteLoaded();
    void onSpriteClosed();
    void onFrameChanged(int frame);

private:
    void rebuild();
    void updateFrame(int frame);
    QLabel* propLabel(const QString& name, const QString& value);

    MainWindow* m_main;
    QWidget* m_content = nullptr;
    QVBoxLayout* m_layout = nullptr;

    QLabel* m_frameIndex = nullptr;
    QLabel* m_frameSize = nullptr;
    QLabel* m_frameOrigin = nullptr;
    QLabel* m_frameInterval = nullptr;
    QLabel* m_playingLabel = nullptr;
};