#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <memory>

namespace Optix::Application {
class AppContext;
}

namespace Optix::UI::Views {

class ReportsView : public QWidget {
    Q_OBJECT

public:
    explicit ReportsView(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~ReportsView() override = default;

    void refreshReport();

private slots:
    void handleFilter();
    void handleExportCSV();

private:
    void setupUI();

    std::shared_ptr<Application::AppContext> m_context;

    QLineEdit* m_searchEdit{nullptr};
    QComboBox* m_deptFilterCombo{nullptr};
    QLineEdit* m_dateFilterEdit{nullptr}; // Simple text filter or QDateEdit later
    
    QTableWidget* m_reportTable{nullptr};
    QPushButton* m_exportBtn{nullptr};
};

} // namespace Optix::UI::Views
