#pragma once
#include <QWidget>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <memory>

namespace Optix::Application {
class AppContext;
}

namespace Optix::UI::Views {

class SettingsView : public QWidget {
    Q_OBJECT

public:
    explicit SettingsView(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~SettingsView() override = default;

private slots:
    void handleThemeToggle();
    void handleThresholdChanged(int value);

private:
    void setupUI();
    void updateThemeBtnText();

    std::shared_ptr<Application::AppContext> m_context;

    QPushButton* m_themeToggleBtn{nullptr};
    QComboBox* m_cameraSelectCombo{nullptr};
    QSlider* m_thresholdSlider{nullptr};
    QLabel* m_thresholdValLbl{nullptr};
};

} // namespace Optix::UI::Views
