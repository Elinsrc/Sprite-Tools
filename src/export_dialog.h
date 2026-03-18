#pragma once

#include <QDialog>
#include <QRadioButton>
#include <QButtonGroup>

#include "app_state.h"
#include "sprite_loader.h"

class ExportDialog : public QDialog
{
    Q_OBJECT
public:
    ExportDialog(QWidget* parent, AppState& state, SpriteLoader& loader);

private slots:
    void onExport();

private:
    AppState& m_state;
    SpriteLoader& m_loader;

    QButtonGroup* m_fmtGroup;
    QRadioButton* m_allFrames;
    QRadioButton* m_currentOnly;
};