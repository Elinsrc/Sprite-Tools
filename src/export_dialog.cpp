#include "export_dialog.h"
#include "sprite_converter.h"
#include "theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QApplication>
#include <QFileInfo>
#include <QFrame>
#include <QSettings>

ExportDialog::ExportDialog(QWidget* parent, AppState& state, SpriteLoader& loader) : QDialog(parent), m_state(state), m_loader(loader)
{
    setWindowTitle("Export Frames");
    setFixedSize(360, 280);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(24, 24, 24, 24);
    lay->setSpacing(0);

    auto* fmtLbl = new QLabel("Output Format");
    fmtLbl->setStyleSheet(QString("font-size: 14px; font-weight: 500; color: %1;").arg(SpriteColors::TextSection.name()));
    lay->addWidget(fmtLbl);
    lay->addSpacing(8);

    m_fmtGroup = new QButtonGroup(this);
    auto* fmtPng = new QRadioButton("PNG (with alpha)");
    auto* fmtBmp = new QRadioButton("BMP (no alpha)");
    fmtPng->setChecked(true);
    m_fmtGroup->addButton(fmtPng, 0);
    m_fmtGroup->addButton(fmtBmp, 1);

    lay->addWidget(fmtPng);
    lay->addSpacing(4);
    lay->addWidget(fmtBmp);
    lay->addSpacing(20);

    auto* selLbl = new QLabel("Frame Selection");
    selLbl->setStyleSheet(QString("font-size: 14px; font-weight: 500; color: %1;").arg(SpriteColors::TextSection.name()));
    lay->addWidget(selLbl);
    lay->addSpacing(8);

    m_allFrames = new QRadioButton(QString("All frames (%1)").arg(state.total_frames));
    m_currentOnly = new QRadioButton(QString("Current frame (%1)").arg(state.current_frame + 1));
    m_allFrames->setChecked(true);

    lay->addWidget(m_allFrames);
    lay->addSpacing(4);
    lay->addWidget(m_currentOnly);

    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(8);
    btnRow->addStretch();

    auto* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(QString(
        "QPushButton { background: transparent; color: %1; border: none; font-weight: 500; padding: 0 12px; height: 36px; }"
        "QPushButton:hover { background: rgba(255, 255, 255, 0.05); border-radius: 4px; }"
    ).arg(SpriteColors::Accent.name()));

    auto* exportBtn = new QPushButton("Export");
    exportBtn->setCursor(Qt::PointingHandCursor);
    exportBtn->setStyleSheet(QString(
        "QPushButton { background: %1; color: #FFFFFF; border: none; border-radius: 4px; font-weight: 500; padding: 0 16px; height: 36px; }"
        "QPushButton:hover { background: %2; }"
    ).arg(SpriteColors::Accent.name(), SpriteColors::AccentLit.name()));

    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(exportBtn);
    lay->addLayout(btnRow);

    connect(exportBtn, &QPushButton::clicked, this, &ExportDialog::onExport);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void ExportDialog::onExport()
{
    QSettings settings("Sprite-Tools");
    QString lastDir = settings.value("lastExportDir", QString::fromStdString(m_state.last_dir)).toString();

    QString dir = QFileDialog::getExistingDirectory(this, "Output Directory", lastDir);
    
    if (dir.isEmpty())
        return;

    settings.setValue("lastExportDir", dir);
    m_state.last_dir = dir.toStdString();

    int format = m_fmtGroup->checkedId();
    int frameIdx = m_currentOnly->isChecked() ? m_state.current_frame : -1;
    accept();

    QProgressDialog prog("Exporting...", "Cancel", 0, 100, parentWidget());
    prog.setWindowTitle("Exporting Frames");
    prog.setWindowModality(Qt::WindowModal);
    prog.setMinimumDuration(0);

    SpriteLoader loader;
    if (!loader.Load(m_state.filepath))
    {
        QMessageBox::critical(parentWidget(), "Error", "Failed to load sprite");
        return;
    }

    QFileInfo fi(QString::fromStdString(m_state.filepath));
    QString prefix = fi.completeBaseName();
    int t = loader.GetTotalFrameCount();
    int start = 0, end = t;
    if (frameIdx >= 0 && frameIdx < t) { start = frameIdx; end = frameIdx + 1; }

    int exported = 0;
    std::string ext = SpriteConverter::GetFormatExtension(static_cast<ImageFormat>(format));

    for (int i = start; i < end; i++)
    {
        if (prog.wasCanceled()) break;
        prog.setLabelText(QString("Exporting frame %1 / %2...").arg(i + 1).arg(end));

        SpriteFrame* frame = loader.GetFrame(i);
        if (!frame || frame->rgba.empty())
        {
            QMessageBox::critical(parentWidget(), "Error", QString("Failed to get frame %1").arg(i));
            return;
        }

        QString outPath = (t == 1 && frameIdx < 0)
            ? QString("%1/%2%3").arg(dir, prefix, QString::fromStdString(ext))
            : QString("%1/%2_%3%4").arg(dir, prefix).arg(i, 3, 10, QChar('0')).arg(QString::fromStdString(ext));

        if (!SpriteConverter::SaveImageRGBA(outPath.toStdString().c_str(),
                frame->rgba.data(), frame->width, frame->height,
                static_cast<ImageFormat>(format)))
        {
            QMessageBox::critical(parentWidget(), "Error", "Failed to save: " + outPath);
            return;
        }
        exported++;
        prog.setValue((int)((float)(i - start + 1) / (end - start) * 100));
        QApplication::processEvents();
    }

    if (!prog.wasCanceled())
        QMessageBox::information(parentWidget(), "Export Complete", QString("Exported %1 file(s) to:\n%2").arg(exported).arg(dir));
}