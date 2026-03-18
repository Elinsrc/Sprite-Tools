#include "properties_panel.h"
#include "mainwindow.h"
#include "sprite_loader.h"
#include "theme.h"

#include <QGroupBox>
#include <QFileInfo>
#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>

QWidget* sectionHeader(const QString& title)
{
    auto* w = new QWidget;
    auto* lay = new QHBoxLayout(w);
    lay->setContentsMargins(0, 12, 0, 4);
    lay->setSpacing(8);

    auto* line1 = new QFrame; line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet(QString("color: %1;").arg(SpriteColors::Border.name()));
    line1->setFixedWidth(12);

    auto* lbl = new QLabel(title);
    lbl->setStyleSheet(QString("font-size: 13px; font-weight: 500; color: %1;").arg(SpriteColors::TextSection.name()));

    auto* line2 = new QFrame; line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet(QString("color: %1;").arg(SpriteColors::Border.name()));

    lay->addWidget(line1);
    lay->addWidget(lbl);
    lay->addWidget(line2);
    return w;
}

class PaletteWidget : public QWidget
{
public:
    PaletteWidget(const uint8_t* pal, int count, QWidget* parent = nullptr)
        : QWidget(parent), m_pal(pal), m_count(count)
    {
        setMouseTracking(true);
        int cols = 16, cs = 10;
        int rows = (count + cols - 1) / cols;
        setFixedSize(cols * (cs + 1), rows * (cs + 1));
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        int cols = 16, cs = 10;
        for (int i = 0; i < m_count && i < 256; i++)
        {
            int r = i / cols, c = i % cols;
            p.fillRect(c * (cs + 1), r * (cs + 1), cs, cs, QColor(m_pal[i*3], m_pal[i*3+1], m_pal[i*3+2]));
        }
    }

    void mouseMoveEvent(QMouseEvent* e) override
    {
        int cols = 16, cs = 10;
        int c = e->pos().x() / (cs + 1);
        int r = e->pos().y() / (cs + 1);
        int idx = r * cols + c;
        if (idx >= 0 && idx < m_count)
        {
            uint8_t rv = m_pal[idx*3], g = m_pal[idx*3+1], b = m_pal[idx*3+2];
            QToolTip::showText(e->globalPosition().toPoint(),
                QString("#%1: %2 %3 %4 (#%5%6%7)")
                    .arg(idx).arg(rv).arg(g).arg(b)
                    .arg(rv, 2, 16, QChar('0'))
                    .arg(g, 2, 16, QChar('0'))
                    .arg(b, 2, 16, QChar('0')));
        }
    }

private:
    const uint8_t* m_pal;
    int m_count;
};

PropertiesPanel::PropertiesPanel(MainWindow* mainWin, QWidget* parent) : QScrollArea(parent), m_main(mainWin)
{
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_content = new QWidget;
    m_layout = new QVBoxLayout(m_content);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setContentsMargins(10, 10, 10, 10);
    m_layout->setSpacing(4);

    auto* ph = new QLabel("No file loaded");
    ph->setStyleSheet(QString("color: %1;").arg(SpriteColors::TextDim.name()));
    ph->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(ph);

    setWidget(m_content);
}

QLabel* PropertiesPanel::propLabel(const QString& name, const QString& value)
{
    return new QLabel(QString("<span style='color:%1;'>%2</span>  %3").arg(SpriteColors::TextDim.name(), name, value));
}

void PropertiesPanel::onSpriteLoaded()  
{ 
    rebuild(); 
}

void PropertiesPanel::onSpriteClosed()
{
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0))) { delete item->widget(); delete item; }
    auto* ph = new QLabel("No file loaded");
    ph->setStyleSheet(QString("color: %1;").arg(SpriteColors::TextDim.name()));
    ph->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(ph);
}
void PropertiesPanel::onFrameChanged(int f) 
{ 
    updateFrame(f); 
}

void PropertiesPanel::rebuild()
{
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0))) { delete item->widget(); delete item; }

    AppState& st = m_main->state();
    SpriteLoader& ld = m_main->loader();
    const SpriteData& d = ld.GetData();

    auto* fileGrp = new QGroupBox("File");
    auto* fileL = new QVBoxLayout(fileGrp);
    auto* fnLbl = new QLabel(QFileInfo(QString::fromStdString(d.filepath)).fileName());
    fnLbl->setToolTip(QString::fromStdString(d.filepath));
    fnLbl->setWordWrap(true);
    fileL->addWidget(fnLbl);
    m_layout->addWidget(fileGrp);

    auto* sprGrp = new QGroupBox("Sprite");
    auto* sprL = new QVBoxLayout(sprGrp);
    const char* vn = (d.version == 1) ? "Quake" : (d.version == 2) ? "Half-Life" : "Unknown";
    sprL->addWidget(propLabel("Version:", QString("%1 (%2)").arg(d.version).arg(vn)));
    sprL->addWidget(propLabel("Type:", SpriteLoader::GetTypeString(d.type)));
    sprL->addWidget(propLabel("Render:", SpriteLoader::GetTexFormatString(d.texFormat)));
    sprL->addWidget(propLabel("Cull:", SpriteLoader::GetFaceTypeString(d.facetype)));
    sprL->addWidget(propLabel("Bounds:", QString("%1 x %2").arg(d.bounds[0]).arg(d.bounds[1])));
    sprL->addWidget(propLabel("Frames:", QString::number(st.total_frames)));
    m_layout->addWidget(sprGrp);

    auto* frGrp = new QGroupBox("Frame");
    auto* frL = new QVBoxLayout(frGrp);
    m_frameIndex = new QLabel;
    m_frameSize = new QLabel;
    m_frameOrigin = new QLabel;
    m_frameInterval = new QLabel;
    frL->addWidget(m_frameIndex);
    frL->addWidget(m_frameSize);
    frL->addWidget(m_frameOrigin);
    frL->addWidget(m_frameInterval);
    m_layout->addWidget(frGrp);

    auto* grpGrp = new QGroupBox("Groups");
    auto* grpL = new QVBoxLayout(grpGrp);
    auto* tree = new QTreeWidget;
    tree->setHeaderHidden(true);
    tree->setMaximumHeight(200);
    tree->setAlternatingRowColors(true);

    int gi = 0;
    for (const auto& g : d.groups)
    {
        const char* ts = (g.type == FRAME_SINGLE) ? "Single" : (g.type == FRAME_GROUP) ? "Group" : "Angled";
        auto* gItem = new QTreeWidgetItem(tree);
        gItem->setText(0, QString("%1 #%2 (%3)").arg(ts).arg(gi).arg(g.frames.size()));
        for (int fi = 0; fi < (int)g.frames.size(); fi++)
        {
            const auto& f = g.frames[fi];
            auto* fItem = new QTreeWidgetItem(gItem);
            fItem->setText(0, QString("%1x%2 (%3,%4) %5s")
                .arg(f.width).arg(f.height)
                .arg(f.origin[0]).arg(f.origin[1])
                .arg(f.interval, 0, 'f', 3));
        }
        gi++;
    }
    grpL->addWidget(tree);
    m_layout->addWidget(grpGrp);

    if (d.version == SPRITE_VERSION_HL && d.palette_colors > 0)
    {
        auto* palGrp = new QGroupBox("Palette");
        auto* palL = new QVBoxLayout(palGrp);
        palL->addWidget(new PaletteWidget(d.palette, d.palette_colors));
        m_layout->addWidget(palGrp);
    }

    m_layout->addStretch();
    updateFrame(st.current_frame);
}

void PropertiesPanel::updateFrame(int frame)
{
    if (!m_frameIndex) 
        return;
    
    AppState& st = m_main->state();
    SpriteFrame* fr = m_main->loader().GetFrame(frame);
    
    if (!fr) 
        return;

    m_frameIndex->setText(propLabel("Index:", QString("%1 / %2").arg(frame + 1).arg(st.total_frames))->text());
    m_frameSize->setText(propLabel("Size:", QString("%1 x %2").arg(fr->width).arg(fr->height))->text());
    m_frameOrigin->setText(propLabel("Origin:", QString("%1, %2").arg(fr->origin[0]).arg(fr->origin[1]))->text());
    m_frameInterval->setText(propLabel("Interval:", QString("%1 s").arg(fr->interval, 0, 'f', 4))->text());
}