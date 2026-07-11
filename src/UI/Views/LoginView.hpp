#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <memory>

namespace Optix::Application {
class AppContext;
}

namespace Optix::UI::Views {

class LoginView : public QWidget {
    Q_OBJECT

public:
    explicit LoginView(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~LoginView() override = default;

signals:
    // Emitted when authentication succeeds
    void loginSuccess();

private slots:
    void handleLogin();

private:
    void setupUI();

    std::shared_ptr<Application::AppContext> m_context;

    // GUI controls
    QLineEdit* m_usernameEdit{nullptr};
    QLineEdit* m_passwordEdit{nullptr};
    QPushButton* m_loginBtn{nullptr};
    QLabel* m_statusLbl{nullptr};
};

} // namespace Optix::UI::Views
