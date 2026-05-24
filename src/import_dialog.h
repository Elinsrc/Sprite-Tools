#pragma once

#include "theme.h"

#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QListWidget>
#include <QStringList>
#include <QToolButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QLineEdit>

#include "app_state.h"

class ImportDialog : public QDialog
{
    Q_OBJECT
public:
    ImportDialog(QWidget* parent, AppState& state);
    QString outputFile() const { return m_outputFile; }

private slots:
    void onAddImages();
    void onClearAll();
    void onRemoveSelected();
    void onCreate();

private:
    AppState& m_state;
    QString m_outputFile;
    QListWidget* m_fileList;
    QComboBox* m_typeCombo;
    QComboBox* m_renderCombo;
    QDoubleSpinBox* m_intervalSpin;
    QStringList m_files;
};

class FileListItem : public QWidget {
public:
    FileListItem(const QString& name, std::function<void()> onRemove) 
    {
        auto* l = new QHBoxLayout(this);

        auto* txt = new QLabel(name);
        txt->setStyleSheet(QString("color: %1;").arg(SpriteColors::TextPrimary.name()));
        
        auto* btn = new QToolButton;
        btn->setIcon(QIcon(":/icons/close.png"));
        btn->setStyleSheet("border: none; background: transparent;");
        btn->setCursor(Qt::PointingHandCursor);
        QObject::connect(btn, &QToolButton::clicked, onRemove);

        l->addWidget(txt, 1);
        l->addWidget(btn);
    }
};