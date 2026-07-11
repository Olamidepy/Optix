#pragma once
#include <QFrame>
#include <QPushButton>
#include <QButtonGroup>
#include <memory>

namespace Optix::Application {
class AppContext;
}

namespace Optix::UI::Common {

class Sidebar : public QFrame {
    Q_OBJECT

public:
    explicit Sidebar(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~Sidebar() override = default;

    // Reset button states (selected page index)
    void setSelectedPage(int index);

signals:
    // Emitted when a navigation menu item is clicked
    void navigationRequested(int pageIndex);

    // Emitted when the logout button is clicked
    void logoutRequested();

private slots:
    void handleButtonClicked(int id);

private:
    void setupUI();
    QPushButton* createNavButton(const QString& text, const QString& icon, int pageId);

    std::shared_ptr<Application::AppContext> m_context;
    QButtonGroup* m_btnGroup{nullptr};
};

} // namespace Optix::UI::Common
