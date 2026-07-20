#pragma once
#include <string_view>

namespace Optix::Core::Styles {

// Define the stylesheet template with placeholders for theme variables.
// These variables will be dynamically replaced when changing themes.
constexpr std::string_view QSS_TEMPLATE = R"(
/* Global Application Styles */
QWidget {
    font-family: 'Inter', sans-serif;
    font-size: 13px;
    color: @text_main;
}

QMainWindow {
    background-color: @bg;
}

/* Dialogs & Popups */
QDialog {
    background-color: @card;
    color: @text_main;
}

QDialog QLabel {
    color: @text_main;
}

/* Sidebar Navigation */
QFrame#SidebarFrame {
    background-color: @card;
    border-right: 1px solid @border;
    min-width: 240px;
    max-width: 240px;
}

QLabel#SidebarTitle {
    font-family: 'Space Grotesk', sans-serif;
    font-size: 22px;
    font-weight: bold;
    color: @primary;
    padding: 20px 15px 5px 15px;
}

QLabel#SidebarSubtitle {
    font-size: 11px;
    font-weight: 500;
    color: @text_muted;
    padding: 0px 15px 20px 15px;
}

/* Sidebar Buttons */
QPushButton#SidebarBtn {
    background-color: transparent;
    color: @text_muted;
    border: none;
    border-radius: 6px;
    padding: 9px 12px;
    text-align: left;
    font-weight: 500;
    margin: 3px 8px;
    font-size: 12px;
}

QPushButton#SidebarBtn:hover {
    background-color: @bg;
    color: @text_main;
}

QPushButton#SidebarBtn:checked {
    background-color: @bg;
    color: @primary;
    font-weight: 600;
}

/* Main Content Area */
QWidget#MainStack {
    background-color: @bg;
}

/* Header Banner */
QLabel#HeaderTitle {
    font-family: 'Space Grotesk', sans-serif;
    font-size: 24px;
    font-weight: bold;
    color: @text_main;
}

QLabel#HeaderSubtitle {
    color: @text_muted;
    font-size: 13px;
}

/* Metric Cards */
QFrame#MetricCard {
    background-color: @card;
    border: 1px solid @border;
    border-radius: 12px;
    padding: 16px;
}

QLabel#MetricVal {
    font-family: 'Space Grotesk', sans-serif;
    font-size: 28px;
    font-weight: bold;
    color: @text_main;
}

QLabel#MetricLbl {
    color: @text_muted;
    font-size: 12px;
    font-weight: 500;
    text-transform: uppercase;
}

/* General Card panels */
QFrame#PanelCard {
    background-color: @card;
    border: 1px solid @border;
    border-radius: 12px;
}

/* Inputs & Form Controls */
QLineEdit {
    background-color: @input_bg;
    border: 1px solid @border;
    border-radius: 8px;
    padding: 10px 14px;
    color: @text_main;
}

QLineEdit:focus {
    border: 1px solid @primary;
}

QLineEdit:disabled {
    background-color: @border;
    color: @text_muted;
}

QComboBox {
    background-color: @input_bg;
    border: 1px solid @border;
    border-radius: 8px;
    padding: 10px 14px;
    color: @text_main;
}

QComboBox:focus {
    border: 1px solid @primary;
}

QComboBox::drop-down {
    border: none;
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 30px;
}

QComboBox QAbstractItemView {
    background-color: @card;
    color: @text_main;
    selection-background-color: @bg;
    selection-color: @primary;
    border: 1px solid @border;
}

/* Table styling */
QTableWidget {
    background-color: @card;
    border: 1px solid @border;
    gridline-color: @border;
    border-radius: 8px;
    outline: 0;
}

QTableWidget::item {
    padding: 12px;
    border-bottom: 1px solid @border;
    color: @text_main;
}

QTableWidget::item:selected {
    background-color: @bg;
    color: @primary;
}

QHeaderView::section {
    background-color: @card;
    color: @text_muted;
    padding: 10px;
    border: none;
    border-bottom: 2px solid @border;
    font-weight: bold;
}

/* Buttons */
QPushButton#PrimaryBtn {
    background-color: @primary;
    color: #FFFFFF !important;
    border: none;
    border-radius: 8px;
    padding: 10px 20px;
    font-weight: 600;
}

QPushButton#PrimaryBtn:hover {
    background-color: @primary_hover;
    color: #FFFFFF !important;
}

QPushButton#PrimaryBtn:pressed {
    background-color: @primary;
    color: #FFFFFF !important;
}

QPushButton#SecondaryBtn {
    background-color: @card;
    color: @text_main;
    border: 1px solid @border;
    border-radius: 8px;
    padding: 10px 20px;
    font-weight: 500;
}

QPushButton#SecondaryBtn:hover {
    background-color: @bg;
    color: @text_main;
}

QPushButton#DestructiveBtn {
    background-color: #EF4444;
    color: #FFFFFF !important;
    border: none;
    border-radius: 8px;
    padding: 10px 20px;
    font-weight: 600;
}

QPushButton#DestructiveBtn:hover {
    background-color: #DC2626;
    color: #FFFFFF !important;
}

/* Login Panel Card */
QFrame#LoginCard {
    background-color: @card;
    border: 1px solid @border;
    border-radius: 16px;
    max-width: 400px;
    max-height: 480px;
}

/* Onboarding Screen Panel */
QFrame#OnboardingCard {
    background-color: @card;
    border: 1px solid @border;
    border-radius: 16px;
    max-width: 500px;
    max-height: 350px;
}

/* Custom Scrollbars */
QScrollBar:vertical {
    border: none;
    background: @bg;
    width: 8px;
    margin: 0px;
}

QScrollBar::handle:vertical {
    background: @border;
    border-radius: 4px;
    min-height: 20px;
}

QScrollBar::handle:vertical:hover {
    background: @text_muted;
}

QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    border: none;
    background: none;
}

/* QMessageBox styling */
QMessageBox {
    background-color: @card;
}

QMessageBox QLabel {
    color: @text_main;
}

QMessageBox QPushButton {
    background-color: @bg;
    color: @text_main;
    border: 1px solid @border;
    border-radius: 6px;
    padding: 6px 16px;
    min-width: 75px;
    font-weight: 500;
}

QMessageBox QPushButton:hover {
    background-color: @primary;
    color: #FFFFFF !important;
    border: 1px solid @primary;
}
)";

} // namespace Optix::Core::Styles
