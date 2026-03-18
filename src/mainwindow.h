#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <QAction>
#include <QDockWidget>
#include <QElapsedTimer>
#include <QToolButton>

#include "app_state.h"
#include "sprite_loader.h"

class SpriteViewport;
class PropertiesPanel;
class FrameSlider;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void openFile(const QString& path);
    AppState& state() { return m_state; }
    SpriteLoader& loader() { return m_loader; }

signals:
    void spriteLoaded();
    void spriteClosed();
    void frameChanged(int frame);
    void zoomChanged(float zoom);

public slots:
    void onOpenFile();
    void onCloseFile();
    void onExport();
    void onImport();
    void onAbout();
    void onPlayPause();
    void onFirstFrame();
    void onLastFrame();
    void onPrevFrame();
    void onNextFrame();
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void onZoomChanged(float z);
    void setFrame(int frame);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QIcon icon(const QString& name);
    QToolButton* tinyBtn(const QString& iconName, const QString& tooltip, bool enabled = true, bool highlighted = false);
    void updateBtnState(QToolButton* btn, bool enabled, bool highlighted = false);

    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createDockWidgets();
    void updateTitle();
    void updateStatusInfo();
    void updatePlayback();
    void animTick();

    AppState m_state;
    SpriteLoader m_loader;

    SpriteViewport* m_viewport;
    PropertiesPanel* m_props;
    QDockWidget* m_propsDock;

    QToolBar* m_toolbar;
    QSlider* m_speedSlider;
    QLabel* m_speedLabel;
    QLabel* m_framePosLabel;
    FrameSlider* m_frameSlider;

    QToolButton* m_btnFirst;
    QToolButton* m_btnPrev;
    QToolButton* m_btnPlayPause;
    QToolButton* m_btnNext;
    QToolButton* m_btnLast;
    QToolButton* m_btnZoomOut;
    QToolButton* m_btnZoomIn;
    QToolButton* m_btnZoomReset;

    QLabel* m_zoomLabel;

    QTimer* m_animTimer;
    QElapsedTimer m_elapsed;

    QLabel* m_statusInfo;
    QLabel* m_statusMsg;

    QAction* m_actOpen;
    QAction* m_actClose;
    QAction* m_actExport;
    QAction* m_actImport;
    QAction* m_actExit;
    QAction* m_actShowToolbar;
    QAction* m_actShowProps;
    QAction* m_actShowChecker;
    
    QMenu* m_menuPlayback;
};