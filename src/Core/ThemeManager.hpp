#pragma once
#include <QObject>
#include <QString>

namespace Optix::Core {

enum class Theme {
    Light,
    Dark
};

class ThemeManager : public QObject {
    Q_OBJECT

public:
    explicit ThemeManager(QObject* parent = nullptr);
    ~ThemeManager() override = default;

    // Get current active theme
    Theme getTheme() const { return m_currentTheme; }

    // Check if Dark theme is active
    bool isDarkMode() const { return m_currentTheme == Theme::Dark; }

    // Toggle between Light and Dark
    void toggleTheme();

    // Set a specific theme
    void setTheme(Theme theme);

    // Get compiled stylesheet based on current theme variables
    QString getStyleSheet() const;

signals:
    // Emitted when the theme is changed
    void themeChanged(Theme newTheme);

private:
    Theme m_currentTheme{Theme::Light};
};

} // namespace Optix::Core
