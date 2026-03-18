#include "import_dialog.h"
#include "sprite_converter.h"
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
#include <QSlider>
#include <QSettings>
#include <QRadioButton>
#include <QLineEdit>
#include <fstream>
#include <stb_image.h>

ImportDialog::ImportDialog(QWidget* parent, AppState& state) : QDialog(parent), m_state(state)
{
    setWindowTitle("Create SPR");
    setFixedSize(450, 580);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(24, 24, 24, 24);
    lay->setSpacing(0);

    auto* inLbl = new QLabel("Input Images");
    inLbl->setStyleSheet(QString("font-size: 14px; font-weight: 500; color: %1;").arg(SpriteColors::TextSection.name()));
    lay->addWidget(inLbl);
    lay->addSpacing(8);

    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(8);
    auto* addBtn = new QPushButton("Add Images...");
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(QString(
        "QPushButton { background: %1; color: %2; border: none; border-radius: 4px; padding: 6px 12px; }"
        "QPushButton:hover { background: %3; }"
    ).arg(SpriteColors::BgLighter.name(), SpriteColors::TextPrimary.name(), SpriteColors::BgLight.name()));

    auto* clrBtn = new QPushButton("Clear");
    clrBtn->setCursor(Qt::PointingHandCursor);
    clrBtn->setStyleSheet(addBtn->styleSheet());

    btnRow->addWidget(addBtn);
    btnRow->addWidget(clrBtn);
    btnRow->addStretch();
    lay->addLayout(btnRow);
    lay->addSpacing(8);

    m_fileList = new QListWidget;
    m_fileList->setFixedHeight(130);
    m_fileList->setStyleSheet(QString(
        "QListWidget { background: %1; border: 1px solid %2; border-radius: 4px; }"
    ).arg(SpriteColors::BgDark.name(), SpriteColors::Border.name()));
    lay->addWidget(m_fileList);
    lay->addSpacing(24);

    auto* setLbl = new QLabel("Sprite Parameters");
    setLbl->setStyleSheet(inLbl->styleSheet());
    lay->addWidget(setLbl);
    lay->addSpacing(12);

    auto addRow = [&](const QString& label, QWidget* w) {
        auto* r = new QHBoxLayout;
        auto* l = new QLabel(label);
        l->setFixedWidth(80);
        l->setStyleSheet(QString("color: %1;").arg(SpriteColors::TextDim.name()));
        r->addWidget(l);
        r->addWidget(w);
        lay->addLayout(r);
        lay->addSpacing(8);
    };

    auto* verW = new QWidget;
    auto* verL = new QHBoxLayout(verW);
    verL->setContentsMargins(0,0,0,0);
    auto* v1 = new QRadioButton("Quake (v1)");
    auto* v2 = new QRadioButton("Half-Life (v2)");
    verL->addWidget(v1);
    verL->addWidget(v2);
    verL->addStretch();
    
    m_versionCombo = new QComboBox;
    m_versionCombo->addItem("v1", 1); 
    m_versionCombo->addItem("v2", 2);
    m_versionCombo->setVisible(false);

    addRow("Version:", verW);

    m_typeCombo = new QComboBox;
    m_typeCombo->addItems({"Parallel Upright", "Facing Upright", "Parallel", "Oriented", "Parallel Oriented"});
    m_typeCombo->setCurrentIndex(2);
    addRow("Type:", m_typeCombo);

    m_renderCombo = new QComboBox;
    m_renderCombo->addItems({"Normal", "Additive", "Index Alpha", "Alpha Test"});
    addRow("Render:", m_renderCombo);

    auto* intW = new QWidget;
    auto* intL = new QHBoxLayout(intW);
    intL->setContentsMargins(0,0,0,0);   
    
    m_intervalSpin = new QDoubleSpinBox;
    m_intervalSpin->setRange(0.01, 1.0);
    m_intervalSpin->setVisible(false);

    auto* intSl = new QSlider(Qt::Horizontal);
    intSl->setRange(1, 100);
    intSl->setFixedHeight(40);
    auto* intLbl = new QLabel("0.10s");
    intLbl->setFixedWidth(40);
    intLbl->setStyleSheet(QString("color: %1;").arg(SpriteColors::TextPrimary.name()));

    intL->addWidget(intSl);
    intL->addWidget(intLbl);
    addRow("Interval:", intW);

    connect(v1, &QRadioButton::toggled, [this](bool c) {
        if(c) {
            m_versionCombo->setCurrentIndex(0);
            m_renderCombo->setCurrentIndex(0);
            m_renderCombo->setEnabled(false);
        }
    });
    connect(v2, &QRadioButton::toggled, [this](bool c) {
        if(c) {
            m_versionCombo->setCurrentIndex(1);
            m_renderCombo->setEnabled(true);
        }
    });

    connect(intSl, &QSlider::valueChanged, [this, intLbl](int v) {
        float val = v / 100.0f;
        m_intervalSpin->setValue((double)val);
        intLbl->setText(QString::number(val, 'f', 2) + "s");
    });

    v2->setChecked(true);
    intSl->setValue(10);
    m_intervalSpin->setValue(0.1);

    lay->addStretch();

    auto* footRow = new QHBoxLayout;
    footRow->setSpacing(8);
    footRow->addStretch();

    auto* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(QString(
        "QPushButton { background: transparent; color: %1; border: none; font-weight: 500; padding: 0 12px; height: 36px; }"
        "QPushButton:hover { background: rgba(255, 255, 255, 0.05); border-radius: 4px; }"
    ).arg(SpriteColors::Accent.name()));

    auto* createBtn = new QPushButton("Create");
    createBtn->setCursor(Qt::PointingHandCursor);
    createBtn->setStyleSheet(QString(
        "QPushButton { background: %1; color: #FFFFFF; border: none; border-radius: 4px; font-weight: 500; padding: 0 16px; height: 36px; }"
        "QPushButton:hover { background: %2; }"
    ).arg(SpriteColors::Accent.name(), SpriteColors::AccentLit.name()));

    footRow->addWidget(cancelBtn);
    footRow->addWidget(createBtn);
    lay->addLayout(footRow);

    connect(addBtn, &QPushButton::clicked, this, &ImportDialog::onAddImages);
    connect(clrBtn, &QPushButton::clicked, this, &ImportDialog::onClearAll);
    connect(createBtn, &QPushButton::clicked, this, &ImportDialog::onCreate);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void ImportDialog::onAddImages()
{
    QSettings settings("Sprite-Tools");
    QString lastDir = settings.value("lastImportDir", QString::fromStdString(m_state.last_dir)).toString();
    QStringList files = QFileDialog::getOpenFileNames(this, "Select Images", lastDir, "Images (*.png *.bmp);;All (*)");

    if (files.isEmpty()) 
        return;

    QString newDir = QFileInfo(files.first()).absolutePath();

    m_state.last_dir = newDir.toStdString();
    
    settings.setValue("lastImportDir", newDir);

    for (const auto& f : files)
    {
        m_files.append(f);
        auto* item = new QListWidgetItem(m_fileList);
        auto* w = new FileListItem(QFileInfo(f).fileName(), [this, item]() {
            int row = m_fileList->row(item);
            m_files.removeAt(row);
            delete m_fileList->takeItem(row);
        });
        item->setSizeHint(w->sizeHint());
        m_fileList->setItemWidget(item, w);
    }
}

void ImportDialog::onClearAll()    
{ 
    m_files.clear();
    m_fileList->clear(); 
}

void ImportDialog::onRemoveSelected()
{
    auto items = m_fileList->selectedItems();
    for (auto* item : items)
    {
        int row = m_fileList->row(item);
        m_files.removeAt(row);
        delete m_fileList->takeItem(row);
    }
}

void ImportDialog::onCreate()
{
    if (m_files.isEmpty())
    {
        QMessageBox::warning(this, "No Images", "Please add at least one image.");
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(this, "Save Sprite", QString::fromStdString(m_state.last_dir), "Sprite (*.spr)");
    if (savePath.isEmpty()) return;
    if (!savePath.endsWith(".spr", Qt::CaseInsensitive)) savePath += ".spr";

    int version = m_versionCombo->itemData(m_versionCombo->currentIndex()).toInt();
    int type = m_typeCombo->currentIndex();
    int texFmt = m_renderCombo->currentIndex();
    float interval = (float)m_intervalSpin->value();

    if (version == 1) texFmt = 0;

    QProgressDialog prog("Creating sprite...", "Cancel", 0, 100, this);
    prog.setWindowTitle("Creating Sprite");
    prog.setWindowModality(Qt::WindowModal);
    prog.setMinimumDuration(0);

    int total = m_files.size();
    std::vector<std::vector<uint8_t>> storage;
    std::vector<const uint8_t*> ptrs;
    std::vector<int> widths, heights;

    for (int i = 0; i < total; i++)
    {
        if (prog.wasCanceled()) { reject(); return; }
        prog.setLabelText(QString("Loading image %1 / %2...").arg(i + 1).arg(total));
        prog.setValue((int)((float)(i + 1) / total * 40));
        QApplication::processEvents();

        int w, h, ch;
        uint8_t* px = stbi_load(m_files[i].toStdString().c_str(), &w, &h, &ch, 4);
        if (!px)
        {
            QMessageBox::critical(this, "Error", "Failed to load: " + QFileInfo(m_files[i]).fileName());
            return;
        }
        storage.emplace_back(px, px + (size_t)w * h * 4);
        stbi_image_free(px);
        widths.push_back(w); 
        heights.push_back(h);
    }

    for (auto& v : storage) ptrs.push_back(v.data());

    prog.setLabelText("Building sprite..."); 
    prog.setValue(50);
    QApplication::processEvents();

    ImageToSprParams p;
    p.version = version; 
    p.type = (uint32_t)type;
    p.tex_format = (uint32_t)texFmt; 
    p.interval = interval;

    auto result = SpriteConverter::RGBAToSprMemory(ptrs, widths, heights, p);
    if (!result.success)
    {
        QMessageBox::critical(this, "Error", QString::fromStdString(result.error));
        return;
    }

    prog.setLabelText("Saving..."); 
    prog.setValue(80);
    QApplication::processEvents();

    std::ofstream file(savePath.toStdString(), std::ios::binary);
    if (!file.is_open())
    {
        QMessageBox::critical(this, "Error", "Failed to create file");
        return;
    }
    file.write(reinterpret_cast<const char*>(result.data.data()), (std::streamsize)result.data.size());
    file.close();

    prog.setValue(100);
    QMessageBox::information(this, "Success", QString("Created: %1\n%2 frame(s), %3 bytes").arg(QFileInfo(savePath).fileName()).arg(total).arg(result.data.size()));

    m_outputFile = savePath;
    accept();
}