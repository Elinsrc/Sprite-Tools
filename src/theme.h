#pragma once

#include <QColor>
#include <QPalette>
#include <QString>

namespace SpriteColors
{
    inline const QColor BgDark      {0x1C, 0x1C, 0x24};
    inline const QColor BgMid       {0x24, 0x24, 0x2B};
    inline const QColor BgLight     {0x2E, 0x2E, 0x38};
    inline const QColor BgLighter   {0x38, 0x38, 0x45};
    inline const QColor ToolbarBg   = BgMid;
    inline const QColor StatusBg    {0x17, 0x17, 0x1F};
    inline const QColor Accent      {0x66, 0x99, 0xF2};
    inline const QColor AccentDim   {0x4D, 0x73, 0xBF};
    inline const QColor AccentLit   {0x8C, 0xB8, 0xFF};
    inline const QColor TextPrimary {0xE0, 0xE0, 0xEB};
    inline const QColor TextDim     {0x80, 0x80, 0x94};
    inline const QColor TextSection {0x8C, 0xA6, 0xE6};
    inline const QColor Border      {0x47, 0x47, 0x59, 0x80};
    inline const QColor Playing     {0x73, 0xD9, 0x73};
    inline const QColor Success     {0x4C, 0xAF, 0x50};
    inline const QColor Error       {0xE5, 0x39, 0x35};
}

namespace Theme
{
    inline QString globalStyleSheet()
    {
        return QStringLiteral(R"(
            * {
                font-family: "Segoe UI", "Noto Sans", "Ubuntu", sans-serif;
            }
            QMainWindow, QDialog {
                background-color: #1C1C24;
            }
            QToolTip {
                color: #E0E0EB;
                background-color: #21212B;
                border: 1px solid #474759;
                padding: 4px 8px;
                border-radius: 4px;
            }
            QMenuBar {
                background-color: #24242B;
                border-bottom: 1px solid #3A3A48;
                padding: 2px;
                color: #E0E0EB;
            }
            QMenuBar::item {
                padding: 4px 10px;
                border-radius: 4px;
            }
            QMenuBar::item:selected {
                background-color: rgba(102, 153, 242, 0.2);
            }
            QMenu {
                background-color: #21212B;
                border: 1px solid #3A3A48;
                border-radius: 8px;
                padding: 4px;
                color: #E0E0EB;
            }
            QMenu::item {
                padding: 6px 24px 6px 12px;
                border-radius: 4px;
            }
            QMenu::item:selected {
                background-color: rgba(102, 153, 242, 0.3);
            }
            QMenu::separator {
                height: 1px;
                background-color: #3A3A48;
                margin: 4px 8px;
            }
            QMenu::icon {
                padding-left: 8px;
            }
            QStatusBar {
                background-color: #17171F;
                border-top: 1px solid #3A3A48;
                color: #808094;
            }
            QStatusBar QLabel {
                padding: 2px 4px;
            }
            QToolBar {
                background-color: #24242B;
                border-bottom: 1px solid #3A3A48;
                spacing: 2px;
                padding: 2px 4px;
            }
            QToolBar QToolButton {
                border: 1px solid transparent;
                border-radius: 6px;
                padding: 4px;
                color: #E0E0EB;
            }
            QToolBar QToolButton:hover {
                background-color: rgba(102, 153, 242, 0.18);
                border: 1px solid rgba(102, 153, 242, 0.25);
            }
            QToolBar QToolButton:pressed {
                background-color: rgba(102, 153, 242, 0.35);
            }
            QToolBar QToolButton:checked {
                background-color: rgba(102, 153, 242, 0.25);
                border: 1px solid rgba(102, 153, 242, 0.4);
            }
            QToolBar QToolButton:disabled {
                color: #505060;
            }
            QToolBar QLabel {
                color: #808094;
                padding: 0 2px;
            }
            QDockWidget {
                color: #8CA6E6;
                font-weight: bold;
            }
            QDockWidget::title {
                background-color: #24242B;
                padding: 8px 12px;
                border-bottom: 1px solid #3A3A48;
                text-align: left;
            }
            QDockWidget::close-button, QDockWidget::float-button {
                border: none;
                padding: 2px;
            }
            QScrollBar:vertical {
                background: #1C1C24;
                width: 10px;
                border: none;
            }
            QScrollBar::handle:vertical {
                background: #505064;
                min-height: 24px;
                border-radius: 4px;
                margin: 2px;
            }
            QScrollBar::handle:vertical:hover {
                background: #6A6A86;
            }
            QScrollBar:horizontal {
                background: #1C1C24;
                height: 10px;
                border: none;
            }
            QScrollBar::handle:horizontal {
                background: #505064;
                min-width: 24px;
                border-radius: 4px;
                margin: 2px;
            }
            QScrollBar::handle:horizontal:hover {
                background: #6A6A86;
            }
            QScrollBar::add-line, QScrollBar::sub-line,
            QScrollBar::add-page, QScrollBar::sub-page {
                background: none;
                height: 0px;
                width: 0px;
            }
            QSlider::groove:horizontal {
                height: 4px;
                background: #3A3A48;
                border-radius: 2px;
            }
            QSlider::handle:horizontal {
                background: #6699F2;
                width: 16px;
                height: 16px;
                margin: -6px 0;
                border-radius: 8px;
            }
            QSlider::handle:horizontal:hover {
                background: #88B4FF;
            }
            QSlider::handle:horizontal:disabled {
                background: #505060;
            }
            QSlider::groove:horizontal:disabled {
                background: #2E2E38;
            }
            QGroupBox {
                font-weight: bold;
                border: 1px solid #3A3A48;
                border-radius: 8px;
                margin-top: 12px;
                padding: 12px 8px 8px 8px;
                color: #8CA6E6;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 12px;
                padding: 0 6px;
            }
            QTreeWidget {
                background-color: #24242C;
                border: 1px solid #3A3A48;
                border-radius: 6px;
                alternate-background-color: #2A2A34;
                color: #E0E0EB;
                outline: none;
            }
            QTreeWidget::item {
                padding: 3px 4px;
                border-radius: 4px;
            }
            QTreeWidget::item:hover {
                background-color: rgba(102, 153, 242, 0.12);
            }
            QTreeWidget::item:selected {
                background-color: rgba(102, 153, 242, 0.25);
            }
            QHeaderView::section {
                background-color: #24242C;
                color: #808094;
                border: none;
                padding: 4px;
            }
            QComboBox {
                background-color: #2E2E38;
                border: 1px solid #3A3A48;
                border-radius: 6px;
                padding: 4px 10px;
                color: #E0E0EB;
                min-height: 28px;
            }
            QComboBox:hover {
                border: 1px solid #6699F2;
            }
            QComboBox::drop-down {
                border: none;
                width: 24px;
            }
            QComboBox QAbstractItemView {
                background-color: #21212B;
                border: 1px solid #3A3A48;
                border-radius: 6px;
                color: #E0E0EB;
                selection-background-color: rgba(102, 153, 242, 0.3);
            }
            QSpinBox, QDoubleSpinBox {
                background-color: #2E2E38;
                border: 1px solid #3A3A48;
                border-radius: 6px;
                padding: 4px 8px;
                color: #E0E0EB;
                min-height: 28px;
            }
            QSpinBox:hover, QDoubleSpinBox:hover {
                border: 1px solid #6699F2;
            }
            QSpinBox::up-button, QSpinBox::down-button,
            QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
                border: none;
                width: 20px;
            }
            QPushButton {
                background-color: #2E2E38;
                border: 1px solid #3A3A48;
                border-radius: 8px;
                padding: 6px 20px;
                color: #E0E0EB;
                min-height: 32px;
                font-weight: 500;
            }
            QPushButton:hover {
                background-color: rgba(102, 153, 242, 0.18);
                border: 1px solid rgba(102, 153, 242, 0.4);
            }
            QPushButton:pressed {
                background-color: rgba(102, 153, 242, 0.35);
            }
            QPushButton:disabled {
                color: #505060;
                background-color: #24242B;
                border-color: #2E2E38;
            }
            QPushButton#primaryButton {
                background-color: #4D73BF;
                border: none;
                color: white;
            }
            QPushButton#primaryButton:hover {
                background-color: #6699F2;
            }
            QPushButton#primaryButton:pressed {
                background-color: #3D5C99;
            }
            QProgressBar {
                border: none;
                border-radius: 4px;
                text-align: center;
                background-color: #2E2E38;
                color: #E0E0EB;
                min-height: 8px;
                max-height: 8px;
            }
            QProgressBar::chunk {
                background-color: #6699F2;
                border-radius: 4px;
            }
            QRadioButton, QCheckBox {
                color: #E0E0EB;
                spacing: 8px;
            }
            QRadioButton::indicator, QCheckBox::indicator {
                width: 18px;
                height: 18px;
            }
            QLineEdit {
                background-color: #2E2E38;
                border: 1px solid #3A3A48;
                border-radius: 6px;
                padding: 4px 8px;
                color: #E0E0EB;
                min-height: 28px;
            }
            QLineEdit:hover {
                border: 1px solid #6699F2;
            }
            QLineEdit:focus {
                border: 2px solid #6699F2;
            }
            QListWidget {
                background-color: #24242C;
                border: 1px solid #3A3A48;
                border-radius: 6px;
                color: #E0E0EB;
                outline: none;
            }
            QListWidget::item {
                padding: 4px 8px;
                border-radius: 4px;
            }
            QListWidget::item:hover {
                background-color: rgba(102, 153, 242, 0.12);
            }
            QListWidget::item:selected {
                background-color: rgba(102, 153, 242, 0.25);
            }
            QTabWidget::pane {
                border: 1px solid #3A3A48;
                border-radius: 6px;
                background-color: #1C1C24;
            }
            QTabBar::tab {
                background-color: #24242B;
                color: #808094;
                padding: 6px 16px;
                border: none;
                border-bottom: 2px solid transparent;
            }
            QTabBar::tab:selected {
                color: #6699F2;
                border-bottom: 2px solid #6699F2;
            }
            QTabBar::tab:hover {
                color: #E0E0EB;
                background-color: rgba(102, 153, 242, 0.1);
            }
            QDialog {
                background-color: #24242B;
                border-radius: 12px;
            }
        )");
    }

    inline QPalette darkPalette()
    {
        QPalette p;
        p.setColor(QPalette::Window, SpriteColors::BgDark);
        p.setColor(QPalette::WindowText, SpriteColors::TextPrimary);
        p.setColor(QPalette::Base, SpriteColors::BgLight);
        p.setColor(QPalette::AlternateBase, SpriteColors::BgLighter);
        p.setColor(QPalette::ToolTipBase, SpriteColors::BgMid);
        p.setColor(QPalette::ToolTipText, SpriteColors::TextPrimary);
        p.setColor(QPalette::Text, SpriteColors::TextPrimary);
        p.setColor(QPalette::Button, SpriteColors::BgLighter);
        p.setColor(QPalette::ButtonText, SpriteColors::TextPrimary);
        p.setColor(QPalette::BrightText, QColor(255, 80, 80));
        p.setColor(QPalette::Link, SpriteColors::Accent);
        p.setColor(QPalette::Highlight, SpriteColors::Accent);
        p.setColor(QPalette::HighlightedText, QColor(0, 0, 0));
        p.setColor(QPalette::Disabled, QPalette::Text, SpriteColors::TextDim);
        p.setColor(QPalette::Disabled, QPalette::ButtonText, SpriteColors::TextDim);
        return p;
    }
}