#include "ThemeManager.hpp"
#include "Styles.hpp"
#include <QApplication>

namespace Optix::Core {

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent), m_currentTheme(Theme::Light) {
}

void ThemeManager::toggleTheme() {
    setTheme(m_currentTheme == Theme::Light ? Theme::Dark : Theme::Light);
}

void ThemeManager::setTheme(Theme theme) {
    if (m_currentTheme == theme) {
        return;
    }
    m_currentTheme = theme;
    
    // Apply compiled stylesheet to the application instance
    if (auto* app = qApp) {
        app->setStyleSheet(getStyleSheet());
    }
    
    emit themeChanged(m_currentTheme);
}

QString ThemeManager::getStyleSheet() const {
    QString qss = QString::fromUtf8(Styles::QSS_TEMPLATE.data(), static_cast<int>(Styles::QSS_TEMPLATE.size()));
    
    if (m_currentTheme == Theme::Light) {
        qss.replace("@bg", "#FAFAFA");
        qss.replace("@card", "#FFFFFF");
        qss.replace("@border", "#ECECEC");
        qss.replace("@primary", "#F97316");
        qss.replace("@primary_hover", "#FB923C");
        qss.replace("@text_main", "#1F2937");
        qss.replace("@text_muted", "#6B7280");
        qss.replace("@input_bg", "#FFFFFF");
    } else {
        qss.replace("@bg", "#121212");
        qss.replace("@card", "#1C1C1C");
        qss.replace("@border", "#2B2B2B");
        qss.replace("@primary", "#F97316");
        qss.replace("@primary_hover", "#FB923C");
        qss.replace("@text_main", "#F3F4F6");
        qss.replace("@text_muted", "#9CA3AF");
        qss.replace("@input_bg", "#242424");
    }
    
    return qss;
}

} // namespace Optix::Core
