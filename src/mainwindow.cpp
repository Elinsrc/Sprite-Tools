#include "mainwindow.h"
#include "sprite_viewport.h"
#include "properties_panel.h"
#include "export_dialog.h"
#include "import_dialog.h"
#include "about_dialog.h"
#include "frame_slider.h"
#include "theme.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QKeyEvent>
#include <QPainter>
#include <QSettings>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("Sprite-Tools");
    setAcceptDrops(true);

    m_viewport = new SpriteViewport(this);
    setCentralWidget(m_viewport);

    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
    createDockWidgets();

    m_animTimer = new QTimer(this);
    m_animTimer->setInterval(16);
    connect(m_animTimer, &QTimer::timeout, this, &MainWindow::animTick);

    connect(this, &MainWindow::spriteLoaded, m_viewport, &SpriteViewport::onSpriteLoaded);
    connect(this, &MainWindow::spriteClosed, m_viewport, &SpriteViewport::onSpriteClosed);
    connect(this, &MainWindow::frameChanged, m_viewport, &SpriteViewport::onFrameChanged);
    connect(this, &MainWindow::zoomChanged,  m_viewport, &SpriteViewport::onZoomChanged);

    connect(this, &MainWindow::spriteLoaded, m_props, &PropertiesPanel::onSpriteLoaded);
    connect(this, &MainWindow::spriteClosed, m_props, &PropertiesPanel::onSpriteClosed);
    connect(this, &MainWindow::frameChanged, m_props, &PropertiesPanel::onFrameChanged);

    connect(m_viewport, &SpriteViewport::zoomRequested, this, &MainWindow::onZoomChanged);

    updatePlayback();
}

MainWindow::~MainWindow()
{
    if (m_state.sprite_loaded)
    {
        m_viewport->deleteTextures();
        m_loader.Unload();
    }
}

QIcon MainWindow::icon(const QString& name)
{
    return QIcon(":/icons/" + name + ".png");
}

QToolButton* MainWindow::tinyBtn(const QString& iconName, const QString& tooltip, bool enabled, bool highlighted)
{
    auto* btn = new QToolButton;
    QIcon ic = icon(iconName);
    
    btn->setProperty("origIcon", QVariant::fromValue(ic));
    
    btn->setIcon(ic);
    btn->setToolTip(tooltip);
    btn->setIconSize(QSize(18, 18));
    btn->setFixedSize(34, 34);
    btn->setAutoRaise(true);
    
    updateBtnState(btn, enabled, highlighted);
    
    return btn;
}

void MainWindow::updateBtnState(QToolButton* btn, bool enabled, bool highlighted)
{
    btn->setEnabled(enabled);

    btn->setStyleSheet(QString(R"(
        QToolButton {
            border: 1px solid transparent;
            border-radius: 6px;
            padding: 4px;
            background: transparent;
        }
        QToolButton:hover {
            background-color: rgba(%1, %2, %3, 46);
            border: 1px solid rgba(%1, %2, %3, 64);
        }
        QToolButton:pressed {
            background-color: rgba(%1, %2, %3, 89);
        }
        QToolButton:disabled {
            background: transparent;
            border: none;
        }
    )").arg(SpriteColors::Accent.red())
       .arg(SpriteColors::Accent.green())
       .arg(SpriteColors::Accent.blue()));

    QColor tint;
    if (!enabled)
    {
        tint = SpriteColors::TextDim; 
    }
    else if (highlighted)
    {
        tint = SpriteColors::AccentLit;
    }
    else
    {
        tint = SpriteColors::TextPrimary;
    }

    QIcon origIcon = btn->property("origIcon").value<QIcon>();
    if (origIcon.isNull())
    {
        origIcon = btn->icon();
        btn->setProperty("origIcon", QVariant::fromValue(origIcon));
    }

    if (!origIcon.isNull())
    {
        QPixmap pm = origIcon.pixmap(18, 18);
        if (!pm.isNull())
        {
            QPainter painter(&pm);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(pm.rect(), tint);
            painter.end();
            btn->setIcon(QIcon(pm));
        }
    }
}

void MainWindow::createActions()
{
    m_actOpen = new QAction(icon("folder"), "&Open...", this);
    m_actOpen->setShortcut(QKeySequence::Open);
    connect(m_actOpen, &QAction::triggered, this, &MainWindow::onOpenFile);

    m_actClose = new QAction(icon("close"), "&Close", this);
    m_actClose->setVisible(false);
    connect(m_actClose, &QAction::triggered, this, &MainWindow::onCloseFile);

    m_actExport = new QAction(icon("save_alt"), "&Export Frames...", this);
    m_actExport->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    m_actExport->setVisible(false); 
    connect(m_actExport, &QAction::triggered, this, &MainWindow::onExport);

    m_actImport = new QAction(icon("add_photo"), "&Import Images to SPR...", this);
    m_actImport->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    connect(m_actImport, &QAction::triggered, this, &MainWindow::onImport);

    m_actExit = new QAction("E&xit", this);
    m_actExit->setShortcut(QKeySequence(Qt::ALT | Qt::Key_F4));
    connect(m_actExit, &QAction::triggered, this, &QWidget::close);

    m_actShowToolbar = new QAction("Toolbar", this);
    m_actShowToolbar->setCheckable(true);
    m_actShowToolbar->setChecked(true);
    connect(m_actShowToolbar, &QAction::toggled, this, [this](bool v) {
        m_toolbar->setVisible(v);
        m_state.show_toolbar = v;
    });

    m_actShowProps = new QAction("Properties", this);
    m_actShowProps->setCheckable(true);
    m_actShowProps->setChecked(true);
    connect(m_actShowProps, &QAction::toggled, this, [this](bool v) {
        m_propsDock->setVisible(v);
        m_state.show_properties = v;
    });

    m_actShowChecker = new QAction("Transparency Grid", this);
    m_actShowChecker->setCheckable(true);
    m_actShowChecker->setChecked(true);
    connect(m_actShowChecker, &QAction::toggled, this, [this](bool v) {
        m_state.show_checker = v;
        m_viewport->update();
    });
}

void MainWindow::createMenus()
{
    QMenu* file = menuBar()->addMenu("&File");
    file->addAction(m_actOpen);
    file->addAction(m_actClose);
    file->addSeparator();
    file->addAction(m_actExport);
    file->addAction(m_actImport);
    file->addSeparator();
    file->addAction(m_actExit);

    QMenu* view = menuBar()->addMenu("&View");
    view->addAction(m_actShowToolbar);
    view->addAction(m_actShowProps);
    view->addSeparator();
    view->addAction(m_actShowChecker);
    view->addSeparator();

    QMenu* zm = view->addMenu("Zoom");
    float zvals[] = {0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f};
    const char* zlbl[] = {"25%", "50%", "100%", "200%", "400%", "800%"};
    for (int i = 0; i < 6; i++)
    {
        float z = zvals[i];
        zm->addAction(zlbl[i], this, [this, z]() { onZoomChanged(z); });
    }

    m_menuPlayback = menuBar()->addMenu("&Playback");
    m_menuPlayback->addAction(icon("play"), "Play/Pause (Space)", this, &MainWindow::onPlayPause);
    m_menuPlayback->addSeparator();
    m_menuPlayback->addAction(icon("skip_previous"),  "First (Home)",     this, &MainWindow::onFirstFrame);
    m_menuPlayback->addAction(icon("navigate_before"),"Previous (Left)",  this, &MainWindow::onPrevFrame);
    m_menuPlayback->addAction(icon("navigate_next"),  "Next (Right)",     this, &MainWindow::onNextFrame);
    m_menuPlayback->addAction(icon("skip_next"),      "Last (End)",       this, &MainWindow::onLastFrame);
    
    m_menuPlayback->menuAction()->setVisible(false);

    QMenu* help = menuBar()->addMenu("&Help");
    help->addAction(icon("info"), "About", this, &MainWindow::onAbout);
}

void MainWindow::createToolBar()
{
    m_toolbar = addToolBar("Main");
    m_toolbar->setMovable(false);
    m_toolbar->setIconSize(QSize(18, 18));
    m_toolbar->setStyleSheet(QString(R"(
        QToolBar {
            background-color: %1;
            border-bottom: 1px solid %2;
            spacing: 2px;
            padding: 4px;
        }
    )").arg(SpriteColors::ToolbarBg.name(), SpriteColors::Border.name()));

    QString sliderStyle = QString(R"(
        QSlider { min-height: 28px; max-height: 28px; }
        QSlider::groove:horizontal {
            height: 4px; background: %1; border-radius: 2px;
        }
        QSlider::sub-page:horizontal {
            background: %2; border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: %3; width: 14px; height: 14px;
            margin: -5px 0; border-radius: 7px;
        }
        QSlider::handle:horizontal:hover { background: %4; }
        QSlider::handle:horizontal:disabled { background: %5; }
        QSlider::groove:horizontal:disabled { background: %6; }
        QSlider::sub-page:horizontal:disabled { background: %6; }
    )").arg(SpriteColors::BgLight.name(), SpriteColors::AccentDim.name(),
            SpriteColors::Accent.name(), SpriteColors::AccentLit.name(),
            SpriteColors::TextDim.name(), SpriteColors::BgDark.name());

    m_btnFirst     = tinyBtn("skip_previous",   "First (Home)");
    m_btnPrev      = tinyBtn("navigate_before", "Previous (Left)");
    m_btnPlayPause = tinyBtn("play",            "Play/Pause (Space)");
    m_btnNext      = tinyBtn("navigate_next",   "Next (Right)");
    m_btnLast      = tinyBtn("skip_next",       "Last (End)");

    connect(m_btnFirst,     &QToolButton::clicked, this, &MainWindow::onFirstFrame);
    connect(m_btnPrev,      &QToolButton::clicked, this, &MainWindow::onPrevFrame);
    connect(m_btnPlayPause, &QToolButton::clicked, this, &MainWindow::onPlayPause);
    connect(m_btnNext,      &QToolButton::clicked, this, &MainWindow::onNextFrame);
    connect(m_btnLast,      &QToolButton::clicked, this, &MainWindow::onLastFrame);

    m_toolbar->addWidget(m_btnFirst);
    m_toolbar->addWidget(m_btnPrev);
    m_toolbar->addWidget(m_btnPlayPause);
    m_toolbar->addWidget(m_btnNext);
    m_toolbar->addWidget(m_btnLast);

    m_speedLabel = new QLabel("1.0x");
    m_speedLabel->setFixedWidth(32);
    m_speedLabel->setAlignment(Qt::AlignCenter);
    m_speedLabel->setStyleSheet(QString("font-size: 10px; color: %1;").arg(SpriteColors::TextDim.name()));

    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(1, 80);
    m_speedSlider->setValue(10);
    m_speedSlider->setFixedWidth(60);
    m_speedSlider->setStyleSheet(sliderStyle);
    m_speedSlider->setToolTip("Playback speed");
    connect(m_speedSlider, &QSlider::valueChanged, this, [this](int v) {
        m_state.anim_speed = v / 10.0f;
        m_speedLabel->setText(QString::number(m_state.anim_speed, 'f', 1) + "x");
    });

    m_toolbar->addWidget(m_speedLabel);
    m_toolbar->addWidget(m_speedSlider);

    auto* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolbar->addWidget(spacer);

    m_btnZoomOut = tinyBtn("zoom_out", "Zoom Out (-)");
    m_btnZoomIn = tinyBtn("zoom_in", "Zoom In (+)");
    
    m_zoomLabel = new QLabel("100%");
    m_zoomLabel->setFixedWidth(45);
    m_zoomLabel->setAlignment(Qt::AlignCenter);
    m_zoomLabel->setStyleSheet(QString("color: %1; font-weight: 500; font-size: 11px;").arg(SpriteColors::TextPrimary.name()));

    m_btnZoomReset = tinyBtn("center_focus", "Reset Zoom (1)");

    connect(m_btnZoomOut, &QToolButton::clicked, this, &MainWindow::onZoomOut);
    connect(m_btnZoomIn, &QToolButton::clicked, this, &MainWindow::onZoomIn);
    connect(m_btnZoomReset, &QToolButton::clicked, this, &MainWindow::onZoomReset);

    m_toolbar->addWidget(m_btnZoomOut);
    m_toolbar->addWidget(m_btnZoomIn);
    m_toolbar->addWidget(m_zoomLabel);
    m_toolbar->addWidget(m_btnZoomReset);

    m_toolbar->addSeparator();

    m_framePosLabel = new QLabel;
    m_framePosLabel->setFixedWidth(48);
    m_framePosLabel->setAlignment(Qt::AlignCenter);
    m_framePosLabel->setStyleSheet(QString("font-size: 11px; color: %1;").arg(SpriteColors::TextDim.name()));
    m_toolbar->addWidget(m_framePosLabel);

    m_frameSlider = new FrameSlider;
    m_frameSlider->setToolTip("Current frame");
    connect(m_frameSlider, &FrameSlider::valueChanged, this, [this](int v) {
        if (v != m_state.current_frame) setFrame(v);
    });
    m_toolbar->addWidget(m_frameSlider);
}

void MainWindow::createStatusBar()
{
    m_statusMsg = new QLabel;
    m_statusInfo = new QLabel;
    m_statusInfo->setStyleSheet(QString("font-size: 10px; color: %1;").arg(SpriteColors::TextDim.name()));
    statusBar()->addWidget(m_statusMsg, 1);
    statusBar()->addPermanentWidget(m_statusInfo);
}

void MainWindow::createDockWidgets()
{
    m_props = new PropertiesPanel(this);
    m_propsDock = new QDockWidget("Properties", this);
    m_propsDock->setWidget(m_props);

    m_propsDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_propsDock->setMinimumWidth(280);
    
    addDockWidget(Qt::RightDockWidgetArea, m_propsDock);

    connect(m_propsDock, &QDockWidget::visibilityChanged, m_actShowProps, &QAction::setChecked);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space)
    {
        onPlayPause();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Left) 
    { 
        onPrevFrame();  
        event->accept(); 
        return; 
    }

    if (event->key() == Qt::Key_Right) 
    { 
        onNextFrame();  
        event->accept(); 
        return; 
    }
    
    if (event->key() == Qt::Key_Home)  
    { 
        onFirstFrame(); 
        event->accept(); 
        return;
    }
    
    if (event->key() == Qt::Key_End)   
    { 
        onLastFrame();  
        event->accept(); 
        return; 
    }

    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal)
    { 
        onZoomIn(); 
        event->accept(); 
        return; 
    }

    if (event->key() == Qt::Key_Minus)
    { 
        onZoomOut(); 
        event->accept(); 
        return; 
    }

    if (event->key() == Qt::Key_1)
    { 
        onZoomReset(); 
        event->accept(); 
        return; 
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::openFile(const QString& path)
{
    if (m_state.sprite_loaded)
    {
        m_viewport->deleteTextures();
        m_loader.Unload();
        m_state.sprite_loaded = false;
    }

    std::string sp = path.toStdString();
    if (!m_loader.Load(sp))
    {
        statusBar()->showMessage("Failed to load: " + QFileInfo(path).fileName(), 4000);
        return;
    }

    m_state.sprite_loaded = true;
    m_state.filepath = sp;
    m_state.fileName = QFileInfo(path).fileName().toStdString();
    m_state.current_frame = 0;
    m_state.total_frames = m_loader.GetTotalFrameCount();
    m_state.animating = false;
    m_state.anim_time = 0.0;
    m_state.scroll_x = m_state.scroll_y = 0;
    m_state.last_dir = QFileInfo(path).absolutePath().toStdString();

    SpriteFrame* f = m_loader.GetFrame(0);
    if (f)
    {
        float m = (float)std::max(f->width, f->height);
        float z = (m < 64) ? 4.0f : (m < 128) ? 2.0f : 1.0f;
        onZoomChanged(z);
    }

    m_frameSlider->setRange(0, std::max(0, m_state.total_frames - 1));
    m_frameSlider->setValue(0);

    m_actClose->setEnabled(true);
    m_actExport->setEnabled(true);

    updateTitle();
    updatePlayback();
    updateStatusInfo();

    statusBar()->showMessage(
        QString("Loaded: %1 (%2 frame(s))")
            .arg(QFileInfo(path).fileName())
            .arg(m_state.total_frames), 4000);

    emit spriteLoaded();
}

void MainWindow::onOpenFile()
{
    QSettings settings("Sprite-Tools");
    QString lastDir = settings.value("lastDir", QString::fromStdString(m_state.last_dir)).toString();

    QString p = QFileDialog::getOpenFileName(this, "Open Sprite", lastDir, "Sprite files (*.spr);;All (*)");

    if (!p.isEmpty()) {
        QString newDir = QFileInfo(p).absolutePath();
        settings.setValue("lastDir", newDir);
        m_state.last_dir = newDir.toStdString();

        openFile(p);
    }
}

void MainWindow::onCloseFile()
{
    if (!m_state.sprite_loaded) 
        return;
    m_animTimer->stop();
    m_state.animating = false;
    m_viewport->deleteTextures();
    m_loader.Unload();
    m_state.sprite_loaded = false;
    m_state.current_frame = 0;
    m_state.total_frames = 0;
    m_actClose->setEnabled(false);
    m_actExport->setEnabled(false);
    m_frameSlider->setRange(0, 0);
    updateTitle();
    updatePlayback();
    updateStatusInfo();
    statusBar()->showMessage("Closed", 3000);
    emit spriteClosed();
    onZoomChanged(1.0f);
}

void MainWindow::onExport()
{
    if (!m_state.sprite_loaded) 
        return;
    ExportDialog dlg(this, m_state, m_loader);
    dlg.exec();
}

void MainWindow::onImport()
{
    ImportDialog dlg(this, m_state);
    if (dlg.exec() == QDialog::Accepted && !dlg.outputFile().isEmpty())
        openFile(dlg.outputFile());
}

void MainWindow::onAbout()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::onPlayPause()
{
    if (!m_state.sprite_loaded || m_state.total_frames <= 1) 
        return;
    
    m_state.animating = !m_state.animating;
    if (m_state.animating)
    {
        m_elapsed.start();
        m_state.anim_time = 0.0;
        m_animTimer->start();
    }
    else
    {
        m_animTimer->stop();
    }
    updatePlayback();
}

void MainWindow::onFirstFrame() 
{ 
    setFrame(0); 
}

void MainWindow::onLastFrame()  
{ 
    setFrame(std::max(0, m_state.total_frames - 1)); 
}

void MainWindow::onPrevFrame()
{
    if (m_state.total_frames <= 0) 
        return;
    setFrame((m_state.current_frame - 1 + m_state.total_frames) % m_state.total_frames);
}

void MainWindow::onNextFrame()
{
    if (m_state.total_frames <= 0) 
        return;
    setFrame((m_state.current_frame + 1) % m_state.total_frames);
}

void MainWindow::onZoomIn()    
{ 
    onZoomChanged(std::min(16.0f, m_state.zoom * 2.0f)); 
}

void MainWindow::onZoomOut()   
{ 
    onZoomChanged(std::max(0.25f, m_state.zoom * 0.5f));
}

void MainWindow::onZoomReset() 
{ 
    m_state.scroll_x = m_state.scroll_y = 0;
    
    SpriteFrame* f = m_loader.GetFrame(0);
    if (f)
    {
        float m = (float)std::max(f->width, f->height);
        float z = (m < 64) ? 4.0f : (m < 128) ? 2.0f : 1.0f;
        onZoomChanged(z);
    }
    else
    {
        onZoomChanged(1.0f);
    }

}

void MainWindow::onZoomChanged(float z)
{
    m_state.zoom = std::clamp(z, 0.25f, 16.0f);
    
    if (m_zoomLabel) 
    {
        m_zoomLabel->setText(QString("%1%").arg(qRound(m_state.zoom * 100)));
    }

    emit zoomChanged(m_state.zoom);
}

void MainWindow::setFrame(int f)
{
    if (f < 0 || f >= m_state.total_frames) return;
    m_state.current_frame = f;

    m_frameSlider->blockSignals(true);
    m_frameSlider->setValue(f);
    m_frameSlider->blockSignals(false);

    m_framePosLabel->setText(QString("%1/%2").arg(f + 1).arg(m_state.total_frames));

    emit frameChanged(f);
    updateStatusInfo();
}

void MainWindow::animTick()
{
    if (!m_state.animating || !m_state.sprite_loaded) return;

    double dt = m_elapsed.elapsed() / 1000.0;
    m_elapsed.restart();
    m_state.anim_time += dt * m_state.anim_speed;

    SpriteFrame* fr = m_loader.GetFrame(m_state.current_frame);
    if (!fr) return;
    float iv = fr->interval > 0.0f ? fr->interval : 0.1f;

    while (m_state.anim_time >= iv)
    {
        m_state.anim_time -= iv;
        m_state.current_frame = (m_state.current_frame + 1) % m_state.total_frames;
        fr = m_loader.GetFrame(m_state.current_frame);
        if (!fr) return;
        iv = fr->interval > 0.0f ? fr->interval : 0.1f;
    }

    m_frameSlider->blockSignals(true);
    m_frameSlider->setValue(m_state.current_frame);
    m_frameSlider->blockSignals(false);

    m_framePosLabel->setText(QString("%1/%2").arg(m_state.current_frame + 1).arg(m_state.total_frames));

    emit frameChanged(m_state.current_frame);
    updateStatusInfo();
}

void MainWindow::updateTitle()
{
    setWindowTitle(m_state.sprite_loaded ? QString::fromUtf8("Sprite-Tools \u2014 ") + QFileInfo(QString::fromStdString(m_state.filepath)).fileName() : "Sprite-Tools");
}

void MainWindow::updateStatusInfo()
{
    if (!m_state.sprite_loaded) 
    { 
        m_statusInfo->clear(); 
        return; 
    }

    const SpriteData& d = m_loader.GetData();
    m_statusInfo->setText(
        QString("v%1  %2  %3 frame(s)")
            .arg(d.version)
            .arg(SpriteLoader::GetTexFormatString(d.texFormat))
            .arg(m_state.total_frames));
}

void MainWindow::updatePlayback()
{
    bool loaded = m_state.sprite_loaded;
    bool multi = loaded && m_state.total_frames > 1;
    bool playing = m_state.animating;

    m_actClose->setVisible(loaded);
    m_actClose->setEnabled(loaded);

    m_actExport->setVisible(loaded);
    m_actExport->setEnabled(loaded);


    updateBtnState(m_btnFirst, multi, false);
    updateBtnState(m_btnPrev,  multi, false);
    updateBtnState(m_btnNext,  multi, false);
    updateBtnState(m_btnLast,  multi, false);

    QIcon playIcon = icon(playing ? "pause" : "play");
    m_btnPlayPause->setProperty("origIcon", QVariant::fromValue(playIcon));
    updateBtnState(m_btnPlayPause, multi, playing);

    updateBtnState(m_btnZoomOut,   true, false);
    updateBtnState(m_btnZoomIn,    true, false);
    updateBtnState(m_btnZoomReset, true, false);

    m_speedSlider->setEnabled(multi);
    m_frameSlider->setEnabled(multi);

    if (loaded)
        m_framePosLabel->setText(QString("%1/%2").arg(m_state.current_frame + 1).arg(m_state.total_frames));
    else
        m_framePosLabel->clear();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
        for (const QUrl& u : event->mimeData()->urls())
            if (u.toLocalFile().endsWith(".spr", Qt::CaseInsensitive))
            { 
                event->acceptProposedAction(); 
                return; 
            }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    for (const QUrl& u : event->mimeData()->urls())
    {
        QString p = u.toLocalFile();
        if (p.endsWith(".spr", Qt::CaseInsensitive))
        { 
            openFile(p); 
            return; 
        }
    }
}