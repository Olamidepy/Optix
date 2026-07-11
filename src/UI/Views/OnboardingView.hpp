#pragma once
#include <QWidget>
#include <QPushButton>
#include <memory>

namespace Optix::Application {
class AppContext;
}

namespace Optix::UI::Views {

class OnboardingView : public QWidget {
    Q_OBJECT

public:
    explicit OnboardingView(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~OnboardingView() override = default;

signals:
    // Emitted when the onboarding action is clicked
    void registerStudentsClicked();

private:
    void setupUI();

    std::shared_ptr<Application::AppContext> m_context;
    QPushButton* m_actionBtn{nullptr};
};

} // namespace Optix::UI::Views
